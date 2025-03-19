#pragma once

#include "ShaderProgram.h"
#include "Utils.h"

class Object;
class SSBO;
class Triangle;

struct RaycastHit
{
	bool hit;
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	Object* object;
	Triangle* triangle;
};

class Physics
{
	inline static UPtr<ComputeShaderProgram> _raycastProgram;
	inline static UPtr<SSBO> _resultSSBO;

	static void init();

public:
	static RaycastHit raycast(glm::vec3 pos, glm::vec3 dir, float maxDis = std::numeric_limits<float>::max());

	friend class BufferController;
	friend class Program;

private:
	struct RaycastHitStruct
	{
		glm::vec3 pos;
		float _pad1;
		glm::vec3 normal;
		float _pad2;
		glm::vec2 uv;
		int objIndex;
		int triIndex;
	};
};
