#include "Physics.h"

#include "Debug.h"
#include "GLObject.h"
#include "Graphical.h"
#include "Object.h"
#include "Scene.h"
#include "Triangle.h"
#include "glm/gtx/string_cast.hpp"

void Physics::init()
{
	raycastProgram = new ComputeShaderProgram("shaders/compute/raycast.comp");
	resultSSBO = new SSBO(sizeof(RaycastHitStruct), 20);
	resultSSBO->setStorage(1);
}
void Physics::uninit()
{
	delete raycastProgram;
	delete resultSSBO;
}

RaycastHit Physics::raycast(glm::vec3 pos, glm::vec3 dir, float maxDis)
{
	raycastProgram->use();
	raycastProgram->setFloat3("rayPos", pos);
	raycastProgram->setFloat3("rayDir", dir);
	raycastProgram->setFloat("rayMaxDis", maxDis);

	ComputeShaderProgram::dispatch({1, 1, 1});
	RaycastHitStruct* result = (RaycastHitStruct*)resultSSBO->mapBuffer();

	auto triangle = result->triIndex != -1 ? Scene::triangles[result->triIndex] : nullptr;
	auto obj = result->objIndex != -1 ? Scene::graphicals[result->objIndex] : triangle ? (Graphical*)triangle->mesh : nullptr;
	RaycastHit hit = {result->normal != glm::vec3(0), result->pos, result->normal, result->uv, obj, triangle};

	resultSSBO->unmapBuffer();
	return hit;
}
