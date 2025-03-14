#pragma once

#include "GLObject.h"
#include "Shader.h"

class TraceShader : public Shader
{
public:
	UBO* uboTexInfos;
	UBO* uboMaterials;
	UBO* uboLights;
	UBO* uboObjects;
	SSBO* ssboTriangles;
	SSBO* ssboBVHNodes;
	SSBO* ssboBVHTriIndices;

	VAO* vaoScreen;

	void setUpScreenTriangles() const;
	TraceShader(const char* path, int id, int);
};