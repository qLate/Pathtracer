#pragma once

#include "glm/vec2.hpp"
#include "RaytraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	inline static bool _renderOneByOne = false;
	inline static bool _limitSamples = false;
	inline static int _samplesPerPixel = 100;
	inline static int _maxRayBounces = 7;

	inline static UPtr<DefaultShaderProgram<RaytraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO;
	inline static UPtr<GLTexture2D> _accumMeanTex;
	inline static UPtr<GLTexture2D> _accumSqrTex;
	inline static UPtr<GLTexture2D> _varianceTex;

	inline static int _frame = 0;
	inline static int _sampleFrame = 0;
	inline static int _totalSamples = 0;
	inline static float _renderTime = -1;

	static void init();

	static void render();
	static void updateCameraUniforms();
	static void updateSetLowSPPIfInteracting();

	static void resizeTextures(glm::ivec2 size);

public:
	static bool renderOneByOne() { return _renderOneByOne; }
	static bool limitSamples() { return _limitSamples; }
	static int samplesPerPixel() { return _samplesPerPixel; }
	static int maxRayBounces() { return _maxRayBounces; }
	static UPtr<DefaultShaderProgram<RaytraceShader>>& renderProgram() { return _renderProgram; }
	static GLFrameBuffer* sceneViewFBO() { return _viewFBO.get(); }

	static float renderTime() { return _renderTime; }

	static void setLimitSamples(bool limit);
	static void setSPP(int samples);
	static void setMaxRayBounces(int bounces);

	static void resizeView(glm::ivec2 size);
	static void resetSamples();

	static float computeSampleVariance();

	friend class Program;
	friend class SDLHandler;

private:
	static constexpr GLenum DRAW_BUFFERS[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};
};
