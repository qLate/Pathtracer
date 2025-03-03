#pragma once

#include "Action.h"
#include "TraceShader.h"

class Pathtracer
{
	inline static constexpr int TRIANGLE_BUFFER_SIZE = 1000;
	inline static constexpr int MAX_RAY_BOUNCE = 5;

	static void initializeScene();

public:
	inline static constexpr int W_WIDTH = 640 * 2;
	inline static constexpr int W_HEIGHT = 360 * 2;
	inline static constexpr glm::ivec2 W_SIZE = {W_WIDTH, W_HEIGHT};
	inline static constexpr glm::vec2 W_RATIO = {W_WIDTH / (float)W_HEIGHT, 1};

	inline static ShaderProgram<TraceShader>* shaderProgram;

	inline static Action onUpdate {};

	static void initialize();
	static void loop();
	static void quit();
};
