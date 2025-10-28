#pragma once

#include "Logging/EditorLog.h"

namespace ARB {
	class Shader
	{
	public:
		unsigned int ID;
		std::string shaderName;
		void recompile(const char* vShaderPath, const char* fShaderPath, std::string name);
		void defaultShaderCompile(std::string name);
		Shader(std::string name);
		void useShader();
		void deleteProgram();
		void setBoolUniform(const std::string, bool value) const;
		void setIntUniform(const std::string, int value) const;
		void setUIntUniform(const std::string, unsigned int value) const;
		void setFloatUniform(const std::string, float value) const;
		void setMatrix4Uniform(const std::string, glm::mat4 value) const;
		void setMatrix3Uniform(const std::string, glm::mat3 value) const;
		void setVec3Uniform(const std::string, glm::vec3 value) const;
		void setVec4Uniform(const std::string, glm::vec4 value) const;
		void setVec2Uniform(const std::string, glm::vec2 value) const;
		void setUIVec3Uniform(const std::string, glm::uvec3 value) const;
		void setUIVec4Uniform(const std::string, glm::uvec4 value) const;
		void setUIVec2Uniform(const std::string, glm::uvec2 value) const;
		void setIVec3Uniform(const std::string, glm::ivec3 value) const;
		void setIVec4Uniform(const std::string, glm::ivec4 value) const;
		void setIVec2Uniform(const std::string, glm::ivec2 value) const;
		void setBoolUniform(const int location, bool value) const;
		void setIntUniform(const int location, int value) const;
		void setUIntUniform(const int location, unsigned int value) const;
		void setFloatUniform(const int location, float value) const;
		void setMatrix4Uniform(const int location, glm::mat4 value) const;
		void setMatrix3Uniform(const int location, glm::mat3 value) const;
		void setVec3Uniform(const int location, glm::vec3 value) const;
		void setVec4Uniform(const int location, glm::vec4 value) const;
		void setVec2Uniform(const int location, glm::vec2 value) const;
		void setUIVec3Uniform(const int location, glm::uvec3 value) const;
		void setUIVec4Uniform(const int location, glm::uvec4 value) const;
		void setUIVec2Uniform(const int location, glm::uvec2 value) const;
		void setIVec3Uniform(const int location, glm::ivec3 value) const;
		void setIVec4Uniform(const int location, glm::ivec4 value) const;
		void setIVec2Uniform(const int location, glm::ivec2 value) const;

		std::string vShaderCode, fShaderCode;
		std::string new_vShaderCode, new_fShaderCode;
	private:
		std::shared_ptr<Editor::Log> shaderLogger;
		int checkStatus(unsigned int obj, std::string type);
	};
}

