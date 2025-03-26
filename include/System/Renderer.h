#pragma once

#include "glm/vec2.hpp"
#include "RaytraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	inline static int _samplesPerPixel = 16;
	inline static int _maxRayBounces = 3;

	inline static UPtr<DefaultShaderProgram<RaytraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO;

	inline static int _frame = 0;

	static void init();

	static void render();
	static void updateCameraUniforms();

public:
	static int samplesPerPixel() { return _samplesPerPixel; }
	static int maxRayBounces() { return _maxRayBounces; }
	static UPtr<DefaultShaderProgram<RaytraceShader>>& renderProgram() { return _renderProgram; }
	static UPtr<GLFrameBuffer>& sceneViewFBO() { return _viewFBO; }

	static void setSamplesPerPixel(int samples);
	static void setMaxRayBounces(int bounces);
	static void resizeView(glm::ivec2 size);
	static void setViewFBO(std::unique_ptr<GLFrameBuffer> fbo);

	friend class Program;
};
