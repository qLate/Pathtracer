#pragma once

#include "glm/vec2.hpp"
#include "TraceShader.h"
#include "ShaderProgram.h"
#include "Utils.h"

class GLFrameBuffer;

class Renderer
{
	static constexpr int TRIANGLE_BUFFER_SIZE = 1000;
	static constexpr int MAX_RAY_BOUNCE = 5;

	static constexpr glm::ivec3 TEX_ARRAY_BOUNDS = {8192, 8192, 32};

	inline static UPtr<DefaultShaderProgram<TraceShader>> _renderProgram;
	inline static UPtr<GLFrameBuffer> _viewFBO;
	inline static UPtr<GLTexture2DArray> _texArray;

	static void init();

	static void render();
	static void bindTextures();
	static void updateCameraUniforms();

public:
	static UPtr<DefaultShaderProgram<TraceShader>>& renderProgram() { return _renderProgram; }
	static UPtr<GLFrameBuffer>& sceneViewFBO() { return _viewFBO; }
	static UPtr<GLTexture2DArray>& texArray() { return _texArray; }

	static void resizeView(glm::ivec2 size);
	static void setViewFBO(std::unique_ptr<GLFrameBuffer> fbo);

	friend class Program;
};
