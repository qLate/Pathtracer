#include "TraceShader.h"

#include "glad.h"

TraceShader::TraceShader(const char* path, int id, int) : Shader(path, id, GL_FRAGMENT_SHADER)
{
	vaoScreen = new VAO();
	setUpScreenTriangles();

	uboMaterials = new UBO(MATERIAL_ALIGN, 1);
	uboLights = new UBO(LIGHT_ALIGN, 2);
	uboObjects = new UBO(OBJECT_ALIGN, 3);
	ssboTriangles = new SSBO(TRIANGLE_ALIGN, 4);
	ssboBVHNodes = new SSBO(BVH_NODE_ALIGN, 5);

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
