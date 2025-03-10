#pragma once

#include "Shader.h"
#include "glm/vec2.hpp"
#include "TraceShader.h"

class GLFrameBuffer;

class Pathtracer
{
	static constexpr int TRIANGLE_BUFFER_SIZE = 1000;
	static constexpr int MAX_RAY_BOUNCE = 5;

public:
	inline static ShaderProgram<TraceShader>* shaderP;
	inline static GLFrameBuffer* sceneViewFBO;

	static void initTraceShader();

	static void traceScene();

	static void resizeView(glm::ivec2 size);

	static void uninit();
};
