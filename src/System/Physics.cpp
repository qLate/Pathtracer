#include "Physics.h"

#include "GLObject.h"
#include "Graphical.h"
#include "Object.h"
#include "Scene.h"
#include "Triangle.h"
#include "glm/gtx/string_cast.hpp"


void Physics::init()
{
	_raycastProgram = make_unique<ComputeShaderProgram>("shaders/compute/raycast.comp");
	_resultSSBO = make_unique<SSBO>((int)sizeof(RaycastHitStruct) / 4, 20);
	_resultSSBO->setStorage(1, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT);
}

RaycastHit Physics::raycast(glm::vec3 pos, glm::vec3 dir, float maxDis)
{
	_raycastProgram->use();
	_raycastProgram->setFloat3("rayPos", pos);
	_raycastProgram->setFloat3("rayDir", dir);
	_raycastProgram->setFloat("rayMaxDis", maxDis);

	ComputeShaderProgram::dispatch({1, 1, 1});
	RaycastHitStruct result = _resultSSBO->readData<RaycastHitStruct>(1)[0];

	auto triangle = result.triIndex != -1 ? Scene::triangles[result.triIndex] : nullptr;
	auto obj = result.objIndex != -1 ? Scene::graphicals[result.objIndex] : triangle ? (Graphical*)triangle->mesh() : nullptr;
	RaycastHit hit = {result.normal != glm::vec3(0), result.pos, result.normal, result.uv, obj, triangle};

	return hit;
}
