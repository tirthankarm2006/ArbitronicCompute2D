#include "ARBpch.h"
#include "ComputeShader.h"

namespace ARB {
	static bool initParamPtr = false;

	void ComputeShader::InitShader(std::string cShaderPath, std::string name) {
		shaderLogger->push_terminal_sink();

		isCompiled = false;
		shaderName = name;
		m_ComputeCode = 
		"#version 460 core\n"
		"layout(rgba32f, binding = 0) uniform image2D imgOutput;\n"
		"layout(location = 0) uniform float TIME;\n"
		"layout(location = 1) uniform float DELTA_TIME;\n"
		"const ivec2 UV = ivec2(gl_GlobalInvocationID.xy);\n"
		"vec4 VALUE = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"void Compute();\n"
		"void main() {\n"
		"Compute();\n"
		"imageStore(imgOutput, UV, VALUE);\n"
		"}\n";

		std::string computeCode;
		std::ifstream creadFile;
		//ensure ifstream objects can throw exceptions:
		creadFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			creadFile.open(cShaderPath);
			std::stringstream cShaderStream;
			cShaderStream << creadFile.rdbuf();
			creadFile.close();
			computeCode = cShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			shaderLogger->logger->error("Compute Shader file unable to read");
			shaderLogger->logger->trace("[Exception] {}", e.code().message());
			computeCode = " ";
		}

		fullShaderCode = m_ComputeCode + computeCode;
		const char* cShaderCode = (fullShaderCode).c_str();
		unsigned int cObj;

		cObj = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cObj, 1, &cShaderCode, NULL);
		glCompileShader(cObj);

		int shader_success = checkStatus(cObj, "Compute");
		if (!shader_success) {
			shaderLogger->logger->error("Unable to create {0} Compute Shader Object", shaderName);
		}

		ID = glCreateProgram();
		glAttachShader(ID, cObj);
		glLinkProgram(ID);
		int prog_success = checkStatus(ID, "Program");

		if (prog_success && shader_success)
			shaderLogger->logger->info("Compute Shader {0} Shader Program is successfully created with Shader {1}", shaderName, cShaderPath);
		else
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);

		glDeleteShader(cObj);

		initParamPtr = true;
		GetUniformVariablesData();

		//Extracting the size of Invocation tensor
		glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, &invocationSize[0]);
	}

	void ComputeShader::recompileShader(std::string cShaderPath, std::string shaderName) {
		glDeleteProgram(ID);

		std::string computeCode;
		std::ifstream creadFile;
		//ensure ifstream objects can throw exceptions:
		creadFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			creadFile.open(cShaderPath);
			std::stringstream cShaderStream;
			cShaderStream << creadFile.rdbuf();
			creadFile.close();
			computeCode = cShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			shaderLogger->logger->error("Compute Shader file unable to read");
			shaderLogger->logger->trace("[Exception] {}", e.code().message());
			computeCode = " ";
		}

		fullShaderCode = m_ComputeCode + computeCode;
		const char* cShaderCode = (fullShaderCode).c_str();
		unsigned int cObj;

		cObj = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(cObj, 1, &cShaderCode, NULL);
		glCompileShader(cObj);

		int success = checkStatus(cObj, "Compute");
		if (!success)
			shaderLogger->logger->error("Unable to recompile {0} Compute Shader Object", shaderName);

		ID = glCreateProgram();
		glAttachShader(ID, cObj);
		glLinkProgram(ID);
		success = checkStatus(ID, "Program");

		if (success)
			shaderLogger->logger->info("Compute Shader {0} Shader Program is successfully recompiled having shader {1}", shaderName, cShaderPath);
		else
			shaderLogger->logger->error("Unable to recompile {0} Shader Program", shaderName);

		GetUniformVariablesData();

		//Extracting the size of Invocation tensor
		glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, &invocationSize[0]);

		glDeleteShader(cObj);
	}

	void ComputeShader::GetUniformVariablesData() {
		if (!ID)
			return;

		ShaderParams* c_params = new ShaderParams();
		paramNum = 0;

		int uniformNums, maxNameLength;
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformNums);
		glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

		shaderLogger->logger->trace("Active Uniform Variables found on Compilation:");
		for (int i = 0; i < uniformNums; i++) {
			std::vector<char> c_name(maxNameLength);
			std::string typeStr;
			GLenum type;
			GLint size;
			GLsizei length;
			glGetActiveUniform(ID, i, maxNameLength, &length, &size, &type, c_name.data());

			std::string name(c_name.data(), length);
			GLint location = glGetUniformLocation(ID, (const char*)c_name.data());

			if (type == GL_BOOL) {
				UniformBool param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->boolParams.size(); i++) {
					if (m_params->boolParams[i].name == name) {
						param.value = m_params->boolParams[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = false;
				c_params->boolParams.push_back(param);
				typeStr = "boolean";
			}
			else if (type == GL_FLOAT) {
				UniformFloat param = UniformFloat();
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->floatParams.size(); i++) {
					if (m_params->floatParams[i].name == name) {
						param.value = m_params->floatParams[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = 0.0;
				c_params->floatParams.push_back(param);
				typeStr = "float";
			}
			else if (type == GL_INT) {
				UniformInt param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->intParams.size(); i++) {
					if (m_params->intParams[i].name == name) {
						param.value = m_params->intParams[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = 0;
				c_params->intParams.push_back(param);
				typeStr = "int";
			}
			else if (type == GL_SAMPLER_2D) {
				UniformSampler2D param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->sampler2DParams.size(); i++) {
					if (m_params->sampler2DParams[i].name == name) {
						param.value = m_params->sampler2DParams[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = -1;
				c_params->sampler2DParams.push_back(param);
				typeStr = "sampler2D";
			}
			else if (type == GL_UNSIGNED_INT) {
				UniformUint param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->uintParams.size(); i++) {
					if (m_params->uintParams[i].name == name) {
						param.value = m_params->uintParams[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = 0;
				c_params->uintParams.push_back(param);
				typeStr = "unsigned_int";
			}
			else if (type == GL_FLOAT_VEC3) {
				UniformVec3 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->vec3Params.size(); i++) {
					if (m_params->vec3Params[i].name == name) {
						param.value = m_params->vec3Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::vec3(0, 0, 0);
				c_params->vec3Params.push_back(param);
				typeStr = "vec3_float";
			}
			else if (type == GL_FLOAT_VEC4) {
				UniformVec4 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->vec4Params.size(); i++) {
					if (m_params->vec4Params[i].name == name) {
						param.value = m_params->vec4Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::vec4(0.0, 0.0, 0.0, 1.0);
				c_params->vec4Params.push_back(param);
				typeStr = "vec4_float";
			}
			else if (type == GL_FLOAT_VEC2) {
				UniformVec2 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->vec2Params.size(); i++) {
					if (m_params->vec2Params[i].name == name) {
						param.value = m_params->vec2Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::vec2(0, 0);
				c_params->vec2Params.push_back(param);
				typeStr = "vec2_float";
			}
			else if (type == GL_INT_VEC2) {
				UniformIvec2 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->ivec2Params.size(); i++) {
					if (m_params->ivec2Params[i].name == name) {
						param.value = m_params->ivec2Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::ivec2(0, 0);
				c_params->ivec2Params.push_back(param);
				typeStr = "vec2_int";
			}
			else if (type == GL_INT_VEC3) {
				UniformIvec3 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->ivec3Params.size(); i++) {
					if (m_params->ivec3Params[i].name == name) {
						param.value = m_params->ivec3Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::ivec3(0, 0, 0);
				c_params->ivec3Params.push_back(param);
				typeStr = "vec3_int";
			}
			else if (type == GL_INT_VEC4) {
				UniformIvec4 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->ivec4Params.size(); i++) {
					if (m_params->ivec4Params[i].name == name) {
						param.value = m_params->ivec4Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::ivec4(0, 0, 0, 1);
				c_params->ivec4Params.push_back(param);
				typeStr = "vec4_int";
			}
			else if (type == GL_UNSIGNED_INT_VEC2) {
				UniformUvec2 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->uvec2Params.size(); i++) {
					if (m_params->uvec2Params[i].name == name) {
						param.value = m_params->uvec2Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::uvec2(0, 0);
				c_params->uvec2Params.push_back(param);
				typeStr = "vec2_unsigned_int";
			}
			else if (type == GL_UNSIGNED_INT_VEC3) {
				UniformUvec3 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->uvec3Params.size(); i++) {
					if (m_params->uvec3Params[i].name == name) {
						param.value = m_params->uvec3Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::uvec3(0, 0, 0);
				c_params->uvec3Params.push_back(param);
				typeStr = "vec3_unsigned_int";
			}
			else if (type == GL_UNSIGNED_INT_VEC4) {
				UniformUvec4 param;
				param.name = name;
				param.shaderlocation = location;
				//Searching whether the variable with same name is already present in array
				bool isFound = false;
				for (int i = 0; i < m_params->uvec4Params.size(); i++) {
					if (m_params->uvec4Params[i].name == name) {
						param.value = m_params->uvec4Params[i].value;
						isFound = true;
						break;
					}
				}
				if (!isFound)
					param.value = glm::uvec4(0, 0, 0, 1);
				c_params->uvec4Params.push_back(param);
				typeStr = "vec4_unsigned_int";
			}
			else
				continue;

			shaderLogger->logger->trace("{0} of type {1} at location = {2}", name, typeStr, location);
			paramNum++;
		}

		DeleteAllParameters();
		m_params = c_params;
	}

	int ComputeShader::checkStatus(unsigned int obj, std::string type) {
		int success;
		char logInfo[1024];
		if (type == "Compute") {
			glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(obj, 1024, NULL, logInfo);
				shaderLogger->logger->error("Could not compile {} shader", type);
				shaderLogger->logger->trace("[ErrorCode] {}", std::string(logInfo));
				return 0;
			}
			else {
				shaderLogger->logger->info("Compiled {} shader", type);
				return 1;
			}
		}
		else if(type == "Program") {
			glGetProgramiv(obj, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(obj, 1024, NULL, logInfo);
				shaderLogger->logger->error("Could not compile Shader Program");
				shaderLogger->logger->trace("[ErrorCode] {}", std::string(logInfo));
				return 0;
			}
			else {
				shaderLogger->logger->info("Compiled Shader Program");
				return 1;
			}
		}
		return 0;
	}

	void ComputeShader::DeleteAllParameters() {
		if (initParamPtr) {
			//Clear all elements from shader parameters
			m_params->floatParams.clear();
			m_params->boolParams.clear();
			m_params->intParams.clear();
			m_params->uintParams.clear();
			m_params->vec3Params.clear();
			m_params->vec4Params.clear();
			m_params->vec2Params.clear();
			m_params->uvec3Params.clear();
			m_params->uvec4Params.clear();
			m_params->uvec2Params.clear();
			m_params->ivec3Params.clear();
			m_params->ivec4Params.clear();
			m_params->ivec2Params.clear();
			m_params->sampler2DParams.clear();
			m_params->matrix3Params.clear();
			m_params->matrix4Params.clear();
		}
	}

	void ComputeShader::DeleteShaderProgram() {
		glDeleteProgram(ID);
	}

	void ComputeShader::useShader() {
		glUseProgram(ID);
	}

	void ComputeShader::setBoolUniform(const std::string name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void ComputeShader::setIntUniform(const std::string name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ComputeShader::setUIntUniform(const std::string name, unsigned int value) const {
		glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ComputeShader::setFloatUniform(const std::string name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ComputeShader::setMatrix4Uniform(const std::string name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void ComputeShader::setMatrix3Uniform(const std::string name, glm::mat3 value) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void ComputeShader::setVec3Uniform(const std::string name, glm::vec3 value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setVec4Uniform(const std::string name, glm::vec4 value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setVec2Uniform(const std::string name, glm::vec2 value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setUIVec3Uniform(const std::string name, glm::uvec3 value) const {
		glUniform3uiv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setUIVec4Uniform(const std::string name, glm::uvec4 value) const {
		glUniform4uiv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setUIVec2Uniform(const std::string name, glm::uvec2 value) const {
		glUniform2uiv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setIVec3Uniform(const std::string name, glm::ivec3 value) const {
		glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setIVec4Uniform(const std::string name, glm::ivec4 value) const {
		glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void ComputeShader::setIVec2Uniform(const std::string name, glm::ivec2 value) const {
		glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void ComputeShader::setBoolUniform(int location, bool value) const {
		glUniform1i(location, (int)value);
	}
	void ComputeShader::setIntUniform(int location, int value) const {
		glUniform1i(location, value);
	}
	void ComputeShader::setUIntUniform(int location, unsigned int value) const {
		glUniform1ui(location, value);
	}
	void ComputeShader::setFloatUniform(int location, float value) const {
		glUniform1f(location, value);
	}
	void ComputeShader::setMatrix4Uniform(int location, glm::mat4 value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void ComputeShader::setMatrix3Uniform(int location, glm::mat3 value) const {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void ComputeShader::setVec3Uniform(int location, glm::vec3 value) const {
		glUniform3fv(location, 1, &value[0]);
	}
	void ComputeShader::setVec4Uniform(int location, glm::vec4 value) const {
		glUniform4fv(location, 1, &value[0]);
	}
	void ComputeShader::setVec2Uniform(int location, glm::vec2 value) const {
		glUniform2fv(location, 1, &value[0]);
	}
	void ComputeShader::setUIVec3Uniform(int location, glm::uvec3 value) const {
		glUniform3uiv(location, 1, &value[0]);
	}
	void ComputeShader::setUIVec4Uniform(int location, glm::uvec4 value) const {
		glUniform4uiv(location, 1, &value[0]);
	}
	void ComputeShader::setUIVec2Uniform(int location, glm::uvec2 value) const {
		glUniform2uiv(location, 1, &value[0]);
	}
	void ComputeShader::setIVec3Uniform(int location, glm::ivec3 value) const {
		glUniform3iv(location, 1, &value[0]);
	}
	void ComputeShader::setIVec4Uniform(int location, glm::ivec4 value) const {
		glUniform4iv(location, 1, &value[0]);
	}
	void ComputeShader::setIVec2Uniform(int location, glm::ivec2 value) const {
		glUniform2iv(location, 1, &value[0]);
	}
}
