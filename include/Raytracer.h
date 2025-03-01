#pragma once

#include "Action.h"
#include "TraceShader.h"

class Raytracer
{
	inline static constexpr int triangleBufferSize = 1000;

	static void initializeSkybox();
	static void initializeScene();
	static void initializeFBO(Shader*& screenShader, unsigned& fbo, unsigned& renderTexture);

public:
	inline static int width, height;

	inline static TraceShader* shader;

	inline static Action onUpdate{};

	static void initialize(int width, int height);
	static void loop();
	static void quit();
};
