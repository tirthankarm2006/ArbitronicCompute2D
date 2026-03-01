#pragma once

#include "ARBpch.h"

#ifndef  _ARB_OPENGLSHADERUNIFORMS_H_
#define _ARB_OPENGLSHADERUNIFORMS_H_
namespace ARB {
	struct UniformFloat {
		UniformFloat() : name("_F_"), shaderlocation(0), value(0.0) {}
		std::string name;
		unsigned int shaderlocation;
		float value;
	};

	struct UniformInt {
		UniformInt() : name("_I_"), shaderlocation(0), value(0) {}
		std::string name;
		unsigned int shaderlocation;
		int value;
	};

	struct UniformUint {
		UniformUint() : name("_UI_"), shaderlocation(0), value(0) {}
		std::string name;
		unsigned int shaderlocation;
		unsigned int value;
	};

	struct UniformBool {
		UniformBool() : name("_B_"), shaderlocation(0), value(false) {}
		std::string name;
		unsigned int shaderlocation;
		bool value;
	};

	struct UniformSampler2D {
		UniformSampler2D() : name("_S2D_"), shaderlocation(0), value(-1) {}
		std::string name;
		unsigned int shaderlocation;
		int value;
	};

	struct UniformVec3 {
		UniformVec3() : name("_VEC3_"), shaderlocation(0), value(glm::vec3(0, 0, 0)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::vec3 value;
	};

	struct UniformVec2 {
		UniformVec2() : name("_VEC2_"), shaderlocation(0), value(glm::vec2(0, 0)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::vec2 value;
	};

	struct UniformVec4 {
		UniformVec4() : name("_VEC4_"), shaderlocation(0), value(glm::vec4(0, 0, 0, 1)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::vec4 value;
	};

	struct UniformIvec3 {
		UniformIvec3() : name("_IVEC3_"), shaderlocation(0), value(glm::ivec3(0, 0, 0)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::ivec3 value;
	};

	struct UniformIvec2 {
		UniformIvec2() : name("_IVEC2_"), shaderlocation(0), value(glm::ivec2(0, 0)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::ivec2 value;
	};

	struct UniformIvec4 {
		UniformIvec4() : name("_IVEC4_"), shaderlocation(0), value(glm::ivec4(0, 0, 0, 1)) {}
		std::string name;
		unsigned int shaderlocation;
		glm::ivec4 value;
	};

	typedef struct {
		std::string name;
		unsigned int shaderlocation;
		glm::uvec3 value;
	}UniformUvec3;

	typedef struct {
		std::string name;
		unsigned int shaderlocation;
		glm::uvec2 value;
	}UniformUvec2;

	typedef struct {
		std::string name;
		unsigned int shaderlocation;
		glm::uvec4 value;
	}UniformUvec4;

	typedef struct {
		std::string name;
		unsigned int shaderlocation;
		glm::mat3 value;
	}UniformMatrix3;

	typedef struct {
		std::string name;
		unsigned int shaderlocation;
		glm::mat4 value;
	}UniformMatrix4;
}
#endif //  _ARB_OPENGLSHADERUNIFORMS_H_
