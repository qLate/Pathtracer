#include "RaytraceShader.h"

#include "BufferController.h"
#include "glad.h"
#include "Renderer.h"

RaytraceShader::RaytraceShader(const char* path, int id, int) : Shader(path, id, GL_FRAGMENT_SHADER)
{
	_vaoScreen = new VAO();
	setUpScreenTriangles();
}

void RaytraceShader::setUpScreenTriangles() const
{
	float screenVertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};
	_vaoScreen->setVertices(screenVertices, 18);

	glBindVertexArray(_vaoScreen->id());
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}
