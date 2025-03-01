#pragma once

#include <vector>

#include "GLObject.h"
#include "Shader.h"

class TraceShader : public Shader
{
public:
	SSBO* ssboLights;
	SSBO* ssboMaterials;
	SSBO* ssboObjects;
	SSBO* ssboTriangles;
	SSBO* ssboBVHNodes;

	VAO* vaoScreen;

	GLCubeMap* skybox;

	std::vector<GLTexture2D*> textures{};

	static constexpr int LIGHT_ALIGN = 20;
	static constexpr int MATERIAL_ALIGN = 12;
	static constexpr int OBJECT_ALIGN = 12;
	static constexpr int TRIANGLE_ALIGN = 44;
	static constexpr int BVH_NODE_ALIGN = 12;

	void setUpScreenTriangles() const;
	TraceShader(const char* vertexPath, const char* fragmentPath);

	void addTexture2D(const Texture* texture);
};
