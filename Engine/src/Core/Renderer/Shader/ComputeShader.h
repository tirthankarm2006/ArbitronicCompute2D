#pragma once

#include "Logging/EditorLog.h"

namespace ARB {
	class ComputeShader
	{
	public:
		unsigned int ID;
		std::string shaderName;
		void InitShader(std::string cShaderPath, std::string name);
		ComputeShader();
		void useShader();
		void GetUniformVariablesData();
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
		void recompileShader(std::string cShaderPath, std::string cShaderName);
		void DeleteAllParameters();
		void DeleteShaderProgram();

		typedef struct {
			std::vector<std::string> paramNames;
			std::vector<int> paramLocs;
			std::vector<GLenum> paramType;
			std::unordered_map<int, float> floatParams;
			std::unordered_map<int, bool> boolParams;
			std::unordered_map<int, int> intParams;
			std::unordered_map<int, unsigned int> uintParams;
			std::unordered_map<int, glm::vec3> vec3Params;
			std::unordered_map<int, glm::vec4> vec4Params;
			std::unordered_map<int, glm::vec2> vec2Params;
			std::unordered_map<int, glm::ivec2> ivec2Params;
			std::unordered_map<int, glm::ivec3> ivec3Params;
			std::unordered_map<int, glm::ivec4> ivec4Params;
			std::unordered_map<int, glm::uvec2> uvec2Params;
			std::unordered_map<int, glm::uvec3> uvec3Params;
			std::unordered_map<int, glm::uvec4> uvec4Params;
		}ShaderParams;

		std::unique_ptr<ShaderParams> params;
		glm::ivec3 invocationSize;
		std::string m_ComputeCode, fullShaderCode;

		bool isCompiled;
	private:
		std::shared_ptr<Editor::Log> shaderLogger;
		int checkStatus(unsigned int obj, std::string type);
	};
}

