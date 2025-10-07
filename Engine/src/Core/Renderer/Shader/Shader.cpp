#include "ARBpch.h"
#include "Shader.h"
#include "Logging/My_Struct_Sink.h"

namespace ARB {
	Shader::Shader(std::string name) {
		shaderLogger = std::make_shared<Editor::Log>("Engine::Renderer3D::Shader");
		shaderName = name;

		const char* vShaderCode = "#version 460 core\n"
			                       "layout(location = 0) in vec3 aPos;\n"
		                           "layout(location = 1) in vec2 aTexCoords;\n"
		                           "out vec3 FragPos;\n"//this is the fragment position
		                           "out vec2 TexCoords;\n"
		                           "void main(){\n"
			                       "gl_Position = vec4(aPos, 1.0);\n"//multipying the transformation matrix with the vectors of our vertices
			                       "TexCoords = aTexCoords; }\n";

		const char* fSHaderCode = "#version 460 core\n"
			                      "out vec4 FragColor;\n"
		                          "in vec3 Normal;\n"
								  "in vec3 FragPos;\n"
								  "in vec2 TexCoords;\n"
								  "struct Material { sampler2D diffuse; };\n"
								  "uniform Material material;\n"
								  "void main() {\n"
			                      "FragColor = vec4(texture(material.diffuse, TexCoords).xyz, 1.0); }\n";

		unsigned int vObj, fObj;

		vObj = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vObj, 1, &vShaderCode, NULL);
		glCompileShader(vObj);

		int success = checkStatus(vObj, "Vertex");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			return;
		}

		fObj = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fObj, 1, &fSHaderCode, NULL);
		glCompileShader(fObj);
		success = checkStatus(fObj, "Fragment");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			return;
		}

		ID = glCreateProgram();
		glAttachShader(ID, vObj);
		glAttachShader(ID, fObj);
		glLinkProgram(ID);
		success = checkStatus(ID, "Program");

		if (success)
			shaderLogger->logger->info("{0} Shader Program is successfully created with Default Shaders", shaderName);
		else {
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
		}

		glDeleteShader(vObj);
		glDeleteShader(fObj);
	}

	void Shader::defaultShaderRecompile( std::string name) {
		glDeleteProgram(ID);

		const char* vShaderCode = "#version 460 core/n"
			"layout(location = 0) in vec3 aPos;/n"
			"layout(location = 1) in vec2 aTexCoords;/n"
			"out vec3 FragPos;/n"//this is the fragment position
			"out vec2 TexCoords;/n"
			"void main(){/n"
			"gl_Position = vec4(aPos, 1.0);/n"//multipying the transformation matrix with the vectors of our vertices
			"TexCoords = aTexCoords; }/n";

		const char* fSHaderCode = "#version 460 core/n"
			"out vec4 FragColor;/n"
			"in vec3 Normal;/n"
			"in vec3 FragPos;/n"
			"in vec2 TexCoords;/n"
			"struct Material { sampler2D diffuse; };/n"
			"uniform Material material;/n"
			"void main() {/n"
			"FragColor = vec4(texture(material.diffuse, TexCoords).xyz, 1.0); }/n";

		unsigned int vObj, fObj;

		vObj = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vObj, 1, &vShaderCode, NULL);
		glCompileShader(vObj);

		int success = checkStatus(vObj, "Vertex");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			return;
		}

		fObj = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fObj, 1, &fSHaderCode, NULL);
		glCompileShader(fObj);
		success = checkStatus(fObj, "Fragment");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			return;
		}

		ID = glCreateProgram();
		glAttachShader(ID, vObj);
		glAttachShader(ID, fObj);
		glLinkProgram(ID);
		success = checkStatus(ID, "Program");

		if (success)
			shaderLogger->logger->info("{0} Shader Program is successfully created with Default Shaders", shaderName);
		else {
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
		}

		glDeleteShader(vObj);
		glDeleteShader(fObj);
	}

	void Shader::recompile(const char* vShaderPath, const char* fShaderPath, std::string name) {
		std::string vertexCode, fragCode;
		std::ifstream vreadFile, freadFile;
		//ensure ifstream objects can throw exceptions:
		vreadFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vreadFile.open(vShaderPath);
			std::stringstream vShaderStream;
			vShaderStream << vreadFile.rdbuf();
			vreadFile.close();
			vertexCode = vShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			shaderLogger->logger->error("Vertex Shader unable to read");
			shaderLogger->logger->trace("[Exception]{}", e.code().message());
			return;
		}

		freadFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			freadFile.open(fShaderPath);
			std::stringstream fShaderStream;
			fShaderStream << freadFile.rdbuf();
			freadFile.close();
			fragCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			shaderLogger->logger->error("Fragment Shader unable to read");
			shaderLogger->logger->trace("[Exception]{}", e.code().message());
			return;
		}

		glDeleteProgram(ID);

		const char* vShaderCode = vertexCode.c_str();
		const char* fSHaderCode = fragCode.c_str();

		unsigned int vObj, fObj;

		vObj = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vObj, 1, &vShaderCode, NULL);
		glCompileShader(vObj);

		int success = checkStatus(vObj, "Vertex");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			return;
		}

		fObj = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fObj, 1, &fSHaderCode, NULL);
		glCompileShader(fObj);
		success = checkStatus(fObj, "Fragment");
		if (!success) {
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			return;
		}

		ID = glCreateProgram();
		glAttachShader(ID, vObj);
		glAttachShader(ID, fObj);
		glLinkProgram(ID);
		success = checkStatus(ID, "Program");

		if (success)
			shaderLogger->logger->info("{0} Shader Program is successfully created with Shaders {1} and {2}", shaderName, vShaderCode, fSHaderCode);
		else {
			glDeleteShader(vObj);
			glDeleteShader(fObj);
			shaderLogger->logger->error("Unable to create {0} Shader Program", shaderName);
		}

		glDeleteShader(vObj);
		glDeleteShader(fObj);
	}

	int Shader::checkStatus(unsigned int obj, std::string type) {
		int success;
		char logInfo[1024];
		if (type != "Program") {
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
		else {
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

	void Shader::useShader() {
		glUseProgram(ID);
	}

	void Shader::setBoolUniform(const std::string name, bool value) const{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);

	}
	void Shader::setIntUniform(const std::string name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);

	}
	void Shader::setFloatUniform(const std::string name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void Shader::setMatrix4Uniform(const std::string name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::setMatrix3Uniform(const std::string name, glm::mat3 value) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));

	}
	void Shader::setVec3Uniform(const std::string name, glm::vec3 value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);

	}
	void Shader::setVec4Uniform(const std::string name, glm::vec4 value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
}
