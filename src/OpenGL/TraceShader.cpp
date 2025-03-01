#include "TraceShader.h"

#include "glad.h"

TraceShader::TraceShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	vaoScreen = new VAO();
	setUpScreenTriangles();

	ssboMaterials = new SSBO(materialAlign, 1);
	ssboLights = new SSBO(lightAlign, 2);
	ssboObjects = new SSBO(objectAlign, 3);
	ssboTriangles = new SSBO(triangleAlign, 4);
	ssboBVHNodes = new SSBO(bvhNodeAlign, 5);

	skybox = new GLCubeMap();
}

void TraceShader::setUpScreenTriangles() const
{
	float screenVertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};
	vaoScreen->setVertices(screenVertices, 18);
	glEnableVertexAttribArray(0);
}


void TraceShader::addTexture2D(const Texture* texture)
{
	textures.push_back(new GLTexture2D(texture));
}

