#pragma once

#include "Logging/EditorLog.h"

namespace ARB {
	class Shader
	{
	public:
		unsigned int ID;
		std::string shaderName;
		void recompile(const char* vShaderPath, const char* fShaderPath, std::string name);
		void defaultShaderRecompile(std::string name);
		Shader(std::string name);
		void useShader();
		void deleteProgram();
		void setBoolUniform(const std::string, bool value) const;
		void setIntUniform(const std::string, int value) const;
		void setFloatUniform(const std::string, float value) const;
		void setMatrix4Uniform(const std::string, glm::mat4 value) const;
		void setMatrix3Uniform(const std::string, glm::mat3 value) const;
		void setVec3Uniform(const std::string, glm::vec3 value) const;
		void setVec4Uniform(const std::string, glm::vec4 value) const;
	private:
		std::shared_ptr<Editor::Log> shaderLogger;
		int checkStatus(unsigned int obj, std::string type);
	};
}

