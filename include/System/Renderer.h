#pragma once

#include "glm/vec2.hpp"
#include "RaytraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	static constexpr int MAX_RAY_BOUNCE = 5;

	inline static UPtr<DefaultShaderProgram<RaytraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO;

	static void init();

	static void render();
	static void updateCameraUniforms();

public:
	static UPtr<DefaultShaderProgram<RaytraceShader>>& renderProgram() { return _renderProgram; }
	static UPtr<GLFrameBuffer>& sceneViewFBO() { return _viewFBO; }

	static void resizeView(glm::ivec2 size);
	static void setViewFBO(std::unique_ptr<GLFrameBuffer> fbo);

	friend class Program;
};
