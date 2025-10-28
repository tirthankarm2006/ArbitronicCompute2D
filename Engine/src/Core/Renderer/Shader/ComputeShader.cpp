#include "ARBpch.h"
#include "ComputeShader.h"

namespace ARB {
	static bool initParamPtr = false;

	void ComputeShader::InitShader(std::string cShaderPath, std::string name) {
		isCompiled = false;
		shaderName = name;
		m_ComputeCode = "#version 460 core\n"
		"layout(rgba32f, binding = 0) uniform image2D imgOutput;\n"
		"layout(location = 0) uniform float TIME;\n"
		"ivec2 UV = ivec2(gl_GlobalInvocationID.xy);\n"
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

		DeleteAllParameters();

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

		//Clear all elements from shader parameters
		DeleteAllParameters();

		GetUniformVariablesData();

		//Extracting the size of Invocation tensor
		glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, &invocationSize[0]);

		glDeleteShader(cObj);
	}

	void ComputeShader::GetUniformVariablesData() {
		int uniformNums, maxNameLength;
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformNums);
		glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

		shaderLogger->logger->trace("Active Uniform variables found:");
		for (int i = 0; i < uniformNums; i++) {
			std::vector<char> name(maxNameLength);

			GLenum type;
			GLint size;
			GLsizei length;
			glGetActiveUniform(ID, i, maxNameLength, &length, &size, &type, name.data());

			std::string nameStr(name.data(), length);

			if (nameStr == "TIME" || nameStr == "imgOutput")
				continue;

			GLint location = glGetUniformLocation(ID, nameStr.c_str());

			if (type == GL_BOOL) {
				params->boolParams.emplace(location, false);
			}
			else if (type == GL_FLOAT) {
				params->floatParams.emplace(location, 0.0);
			}
			else if (type == GL_INT) {
				params->intParams.emplace(location, 0);
			}
			else if (type == GL_UNSIGNED_INT) {
				params->uintParams.emplace(location, 0);
			}
			else if (type == GL_FLOAT_VEC3) {
				params->vec3Params.emplace(location, glm::vec3(0.0, 0.0, 0.0));
			}
			else if (type == GL_FLOAT_VEC4) {
				params->vec4Params.emplace(location, glm::vec4(0.0, 0.0, 0.0, 1.0));
			}
			else if (type == GL_FLOAT_VEC2) {
				params->vec2Params.emplace(location, glm::vec2(0.0, 0.0));
			}
			else if (type == GL_INT_VEC2) {
				params->ivec2Params.emplace(location, glm::ivec2(0, 0));
			}
			else if (type == GL_INT_VEC3) {
				params->ivec3Params.emplace(location, glm::ivec3(0, 0, 0));
			}
			else if (type == GL_INT_VEC4) {
				params->ivec4Params.emplace(location, glm::ivec4(0, 0, 0, 1));
			}
			else if (type == GL_UNSIGNED_INT_VEC2) {
				params->uvec2Params.emplace(location, glm::uvec2(0, 0));
			}
			else if (type == GL_UNSIGNED_INT_VEC3) {
				params->uvec3Params.emplace(location, glm::uvec3(0, 0, 0));
			}
			else if (type == GL_UNSIGNED_INT_VEC4) {
				params->uvec4Params.emplace(location, glm::uvec4(0, 0, 0, 1));
			}
			else
				continue;

			params->paramNames.push_back(nameStr);
			params->paramType.push_back(type);
			params->paramLocs.push_back(location);

			shaderLogger->logger->trace((nameStr + " at location = {0}"), location);
		}
	}

	ComputeShader::ComputeShader() {
		ID = 0;
		isCompiled = false;
		invocationSize = glm::ivec3(1, 1, 1);
		params = std::make_unique<ShaderParams>();
		shaderLogger = std::make_shared<Editor::Log>("Engine::Compute_Shader");
		shaderLogger->push_terminal_sink();
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
			glGetProgramiv(obj, GL_COMPILE_STATUS, &success);
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
	}

	void ComputeShader::DeleteAllParameters() {
		if (initParamPtr) {
			//Clear all elements from shader parameters
			params->floatParams.clear();
			params->boolParams.clear();
			params->intParams.clear();
			params->uintParams.clear();
			params->vec3Params.clear();
			params->vec4Params.clear();
			params->vec2Params.clear();
			params->uvec3Params.clear();
			params->uvec4Params.clear();
			params->uvec2Params.clear();
			params->ivec3Params.clear();
			params->ivec4Params.clear();
			params->ivec2Params.clear();

			params->paramType.clear();
			params->paramNames.clear();
			params->paramLocs.clear();
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
