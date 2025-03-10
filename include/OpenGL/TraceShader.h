#pragma once

#include "GLObject.h"
#include "Shader.h"

class TraceShader : public Shader
{
public:
	UBO* uboLights;
	UBO* uboMaterials;
	UBO* uboObjects;
	SSBO* ssboTriangles;
	SSBO* ssboBVHNodes;

	VAO* vaoScreen;

	GLCubeMap* skybox;

	static constexpr int LIGHT_ALIGN = 16;
	static constexpr int MATERIAL_ALIGN = 12;
	static constexpr int OBJECT_ALIGN = 12;
	static constexpr int TRIANGLE_ALIGN = 40;
	static constexpr int BVH_NODE_ALIGN = 12;

	void setUpScreenTriangles() const;
	TraceShader(const char* path, int id, int);
};
