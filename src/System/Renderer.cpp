#include "Renderer.h"

#include "BufferController.h"
#include "Camera.h"
#include "GLObject.h"
#include "ImGuiHandler.h"
#include "Material.h"

void Renderer::init()
{
	_renderProgram = make_unique<DefaultShaderProgram<RaytraceShader>>("shaders/common/pathtracer.vert", "shaders/pathtracer.frag");
	_renderProgram->use();
	_renderProgram->setFloat2("pixelSize", ImGuiHandler::INIT_RENDER_SIZE);
	setSamplesPerPixel(_samplesPerPixel);
	setMaxRayBounces(_maxRayBounces);
}

void Renderer::render()
{
	_renderProgram->use();
	_renderProgram->setInt("frame", _frame++);

	updateCameraUniforms();

	glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());

	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::updateCameraUniforms()
{
	_renderProgram->setFloat3("cameraPos", Camera::instance->pos());
	_renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->rot()));
}

void Renderer::setSamplesPerPixel(int samples)
{
	_samplesPerPixel = samples;
	_renderProgram->use();
	_renderProgram->setInt("samplesPerPixel", _samplesPerPixel);
}
void Renderer::setMaxRayBounces(int bounces)
{
	_maxRayBounces = bounces;
	_renderProgram->use();
	_renderProgram->setInt("maxRayBounces", _maxRayBounces);
}
void Renderer::resizeView(glm::ivec2 size)
{
	_viewFBO.reset();
	_viewFBO = make_unique<GLFrameBuffer>(size);

	_renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setViewSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);
}

void Renderer::setViewFBO(std::unique_ptr<GLFrameBuffer> fbo)
{
	_viewFBO = std::move(fbo);
}
