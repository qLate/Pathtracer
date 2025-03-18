#pragma once

#include "GLObject.h"
#include "Shader.h"

class TraceShader : public Shader
{
public:
	VAO* vaoScreen;

	void setUpScreenTriangles() const;
	TraceShader(const char* path, int id, int);
};