#include "TraceShader.h"

#include "BufferController.h"
#include "glad.h"

TraceShader::TraceShader(const char* path, int id, int) : Shader(path, id, GL_FRAGMENT_SHADER)
{
	vaoScreen = new VAO();
	setUpScreenTriangles();

	uboTexInfos = new UBO(BufferController::TEX_INFOS_ALIGN, 1);
	uboMaterials = new UBO(BufferController::MATERIAL_ALIGN, 2);
	uboLights = new UBO(BufferController::LIGHT_ALIGN, 3);
	uboObjects = new UBO(BufferController::OBJECT_ALIGN, 4);
	ssboTriangles = new SSBO(BufferController::TRIANGLE_ALIGN, 5);
	ssboBVHNodes = new SSBO(BufferController::BVH_NODE_ALIGN, 6);

	skybox = new GLCubeMap();
}

void TraceShader::setUpScreenTriangles() const
{
	float screenVertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};
	vaoScreen->setVertices(screenVertices, 18);

	glBindVertexArray(vaoScreen->id);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}
