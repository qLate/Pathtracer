#pragma once

#include "glm/vec2.hpp"
#include "RaytraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	static constexpr bool RENDER_ONCE = true;
	inline static int _samplesPerPixel = RENDER_ONCE ? 100 : 1;
	inline static int _maxRayBounces = 7;

	inline static UPtr<DefaultShaderProgram<RaytraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO1;
	inline static UPtr<GLFrameBuffer> _viewFBO2;

	inline static int _frame = 0;
	inline static int _accumFrame = 0;

	static void init();
	static void initViewFBO();

	static void render();
	static void updateCameraUniforms();

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
