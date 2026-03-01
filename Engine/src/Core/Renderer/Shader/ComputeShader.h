#pragma once

#include "Editor/Logging/EditorLog.h"
#include "ShaderUniforms.h"

namespace ARB {
	class ComputeShader
	{
	public:
		unsigned int ID;
		std::string shaderName;
		void InitShader(std::string cShaderPath, std::string name);
		ComputeShader()
			: ID(0), isCompiled(false), invocationSize(glm::ivec3(1, 1, 1)), m_params(new ShaderParams()),
			shaderLogger(std::make_shared<Editor::Log>("Engine::Compute_Shader")), paramNum(0)
		{}
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
		inline int getUniformParamsNumbers() const { return paramNum; }
		void DeleteAllParameters();
		void DeleteShaderProgram();

		struct ShaderParams {
			ShaderParams() {}
			std::vector<UniformFloat> floatParams;
			std::vector<UniformBool> boolParams;
			std::vector<UniformInt> intParams;
			std::vector<UniformUint> uintParams;
			std::vector<UniformSampler2D> sampler2DParams;
			std::vector<UniformVec3> vec3Params;
			std::vector<UniformVec4> vec4Params;
			std::vector<UniformVec2> vec2Params;
			std::vector<UniformIvec2> ivec2Params;
			std::vector<UniformIvec3> ivec3Params;
			std::vector<UniformIvec4> ivec4Params;
			std::vector<UniformUvec2> uvec2Params;
			std::vector<UniformUvec3> uvec3Params;
			std::vector<UniformUvec4> uvec4Params;
			std::vector<UniformMatrix3> matrix3Params;
			std::vector<UniformMatrix4> matrix4Params;
		};

		ShaderParams* m_params;
		glm::ivec3 invocationSize;
		std::string m_ComputeCode, fullShaderCode;

		bool isCompiled;
	private:
		int paramNum;
		std::shared_ptr<Editor::Log> shaderLogger;
		int checkStatus(unsigned int obj, std::string type);
	};
}

