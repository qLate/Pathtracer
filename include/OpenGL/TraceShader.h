#pragma once

#include "GLObject.h"
#include "Shader.h"

class TraceShader : public Shader
{
	VAO* _vaoScreen;

public:
	TraceShader(const char* path, int id, int);
	void setUpScreenTriangles() const;

	VAO* vaoScreen() const { return _vaoScreen; }
};
