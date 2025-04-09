#include "Physics.h"

#include "GLObject.h"
#include "Graphical.h"
#include "Scene.h"
#include "Triangle.h"
#include "WindowDrawer.h"

void Physics::init()
{
	_raycastProgram = make_unique<ComputeShaderProgram>("shaders/compute/raycast.comp");
	_resultSSBO = make_unique<SSBO>((int)sizeof(RaycastHitStruct) / 4, 20);
	_resultSSBO->setStorage(1, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT);
}

RaycastHit Physics::raycast(glm::vec3 pos, glm::vec3 dir, float maxDis)
{
	_raycastProgram->use();
	BufferController::bindBuffers();

	_raycastProgram->setFloat3("rayPos", pos);
	_raycastProgram->setFloat3("rayDir", dir);
	_raycastProgram->setFloat("rayMaxDis", maxDis);
	_raycastProgram->setInt("objectCount", Scene::graphicals.size());
	_raycastProgram->setInt("primObjCount", BufferController::lastPrimObjCount());
	_raycastProgram->setInt("lightCount", Scene::lights.size());
	_raycastProgram->setBool("doIntersectLights", WindowDrawer::showIcons());

	ComputeShaderProgram::dispatch({1, 1, 1});
	RaycastHitStruct result = _resultSSBO->readData<RaycastHitStruct>(1)[0];

	bool hit = result.objIndex != -1;
	auto hitObj = result.objIndex != -1 ? result.hitLight ? (Object*)Scene::lights[result.objIndex] : (Object*)Scene::graphicals[result.objIndex] : nullptr;
	auto triangle = result.triIndex != -1 ? Scene::baseTriangles[result.triIndex] : nullptr;

	return {hit, result.pos, result.normal, result.uv, hitObj, triangle, result.hitLight};
}
