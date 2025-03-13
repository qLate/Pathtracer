#pragma once
#include "ShaderProgram.h"

class SSBO;
class Triangle;

struct Raycast
{
	glm::vec3 hitPos;
	glm::vec3 hitNormal;
	glm::vec2 hitUV;
};

class Physics
{
	inline static ComputeShaderProgram* raycastProgram;
	inline static SSBO* resultSSBO;

public:
	static void init();
	static void uninit();

	static Triangle* raycast(glm::vec3 pos, glm::vec3 dir, float maxDis = std::numeric_limits<float>::max());
};
