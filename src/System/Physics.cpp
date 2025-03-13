#include "Physics.h"

#include "Debug.h"
#include "GLObject.h"
#include "glm/gtx/string_cast.hpp"

void Physics::init()
{
	raycastProgram = new ComputeShaderProgram("shaders/compute/raycast.comp");
	resultSSBO = new SSBO(sizeof(Raycast), 20);
	resultSSBO->setStorage(1);
}
void Physics::uninit()
{
	delete raycastProgram;
	delete resultSSBO;
}

Triangle* Physics::raycast(glm::vec3 pos, glm::vec3 dir, float maxDis)
{
	raycastProgram->use();
	raycastProgram->setFloat3("rayPos", pos);
	raycastProgram->setFloat3("rayDir", dir);
	raycastProgram->setFloat("rayMaxDis", maxDis);

	ComputeShaderProgram::dispatch({1, 1, 1});

	Raycast* result = (Raycast*)resultSSBO->mapBuffer();
	Debug::log("Hit at: " + to_string(result->hitPos) + " normal: " + to_string(result->hitNormal));
	resultSSBO->unmapBuffer();

	return nullptr;
}
