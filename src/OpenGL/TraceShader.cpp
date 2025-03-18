#include "TraceShader.h"

#include "BufferController.h"
#include "glad.h"
#include "Renderer.h"

TraceShader::TraceShader(const char* path, int id, int) : Shader(path, id, GL_FRAGMENT_SHADER)
{
	vaoScreen = new VAO();
	setUpScreenTriangles();
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
