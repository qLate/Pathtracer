#include "Physics.h"

#include "GLObject.h"
#include "Graphical.h"
#include "Scene.h"
#include "Triangle.h"
#include "WindowDrawer.h"
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
	_raycastProgram->setInt("objectCount", Scene::graphicals.size());
	_raycastProgram->setInt("lightCount", Scene::lights.size());
	_raycastProgram->setBool("doIntersectLights", WindowDrawer::showIcons());

	ComputeShaderProgram::dispatch({1, 1, 1});
	RaycastHitStruct result = _resultSSBO->readData<RaycastHitStruct>(1)[0];

	bool hit = result.normal != glm::vec3(0);
	auto triangle = result.triIndex != -1 ? Scene::triangles[result.triIndex] : nullptr;
	auto hitObj = result.objIndex != -1 ? result.hitLight ? (Object*)Scene::lights[result.objIndex] : (Object*)Scene::graphicals[result.objIndex] : nullptr;
	auto finalObj = triangle == nullptr || result.hitLight ? hitObj : (Graphical*)triangle->mesh();

	return {hit, result.pos, result.normal, result.uv, finalObj, triangle, result.hitLight};
}
