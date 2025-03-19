#include "Renderer.h"

#include "Camera.h"
#include "GLObject.h"
#include "ImGUIHandler.h"
#include "Material.h"
#include "MyMath.h"

void Renderer::init()
{
	_renderProgram = make_unique<DefaultShaderProgram<TraceShader>>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	_renderProgram->use();
	_renderProgram->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	_renderProgram->setFloat2("pixelSize", ImGUIHandler::INIT_RENDER_SIZE);

	_texArray = make_unique<GLTexture2DArray>(TEX_ARRAY_BOUNDS.x, TEX_ARRAY_BOUNDS.y, TEX_ARRAY_BOUNDS.z, GL_RGBA8);
	_renderProgram->setFloat2("texArrayBounds", glm::vec2(TEX_ARRAY_BOUNDS.x, TEX_ARRAY_BOUNDS.y));
	_renderProgram->setInt("texArray", 0);
}

void Renderer::render()
{
	_renderProgram->use();

	bindTextures();
	updateCameraUniforms();

	glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());

	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::bindTextures()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, _texArray->id());
}
void Renderer::updateCameraUniforms()
{
	_renderProgram->setFloat3("cameraPos", Camera::instance->pos());
	_renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->rot()));
}

void Renderer::resizeView(glm::ivec2 size)
{
	_viewFBO.reset();
	_viewFBO = make_unique<GLFrameBuffer>(size);

	_renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);
}

void Renderer::setViewFBO(std::unique_ptr<GLFrameBuffer> fbo)
{
	_viewFBO = std::move(fbo);
}
