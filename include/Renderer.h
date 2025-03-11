#pragma once

#include "Shader.h"
#include "glm/vec2.hpp"
#include "TraceShader.h"

class GLFrameBuffer;

class Renderer
{
	static constexpr int TRIANGLE_BUFFER_SIZE = 1000;
	static constexpr int MAX_RAY_BOUNCE = 5;

	static constexpr glm::vec3 TEX_ARRAY_BOUNDS = {8192, 8192, 32};

public:
	inline static ShaderProgram<TraceShader>* shaderP;
	inline static GLFrameBuffer* sceneViewFBO;
	inline static GLTexture2DArray* texArray;

	static void init();
	static void uninit();

	static void render();
	static void bindTextures();

	static void resizeView(glm::ivec2 size);
};
