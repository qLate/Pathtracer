#pragma once

#include "Action.h"
#include "TraceShader.h"

class Pathtracer
{
	inline static constexpr int TRIANGLE_BUFFER_SIZE = 1000;
	inline static constexpr int MAX_RAY_BOUNCE = 5;

public:
	inline static ShaderProgram<TraceShader>* traceShaderP;
	inline static GLFrameBuffer* sceneViewFBO;

	inline static Action onUpdate {};

	static void initialize();

	static void loop();
	static void traceScene();

	static void quit();
};
