#pragma once

#include "glm/vec2.hpp"
#include "RaytraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	inline static bool _renderOnce = false;
	inline static int _samplesPerPixel = _renderOnce ? 100 : 1;
	inline static int _maxRayBounces = 7;

	inline static UPtr<DefaultShaderProgram<RaytraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO1;
	inline static UPtr<GLFrameBuffer> _viewFBO2;
	inline static UPtr<GLTexture2D> _accumSqrTex;
	inline static UPtr<GLTexture2D> _varianceTex;

	inline static int _frame = 0;
	inline static int _accumFrame = 0;

	static void init();

	static void render();
	static void updateCameraUniforms();

	static float computeAccumVariance();

	static void resizeTextures(glm::ivec2 size);

public:
	static int samplesPerPixel() { return _samplesPerPixel; }
	static int maxRayBounces() { return _maxRayBounces; }
	static UPtr<DefaultShaderProgram<RaytraceShader>>& renderProgram() { return _renderProgram; }
	static GLFrameBuffer* sceneViewFBO();

	static void setSamplesPerPixel(int samples);
	static void setMaxRayBounces(int bounces);

	static void resizeView(glm::ivec2 size);
	static void resetAccumulation();

	friend class Program;
	friend class SDLHandler;
};
