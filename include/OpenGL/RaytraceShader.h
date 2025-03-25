#pragma once

#include "GLObject.h"
#include "Shader.h"

class RaytraceShader : public Shader
{
	VAO* _vaoScreen;

public:
	RaytraceShader(const char* path, int id, int);
	void setUpScreenTriangles() const;

	VAO* vaoScreen() const { return _vaoScreen; }
};
