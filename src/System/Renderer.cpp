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

void Renderer::initViewFBO()
{
	_viewFBO1 = std::make_unique<GLFrameBuffer>(ImGuiHandler::INIT_RENDER_SIZE);
	_viewFBO2 = std::make_unique<GLFrameBuffer>(ImGuiHandler::INIT_RENDER_SIZE);
}

void Renderer::render()
{
	_renderProgram->use();
	_renderProgram->setInt("frame", _frame);
	_renderProgram->setInt("accumFrame", _accumFrame);

	updateCameraUniforms();

	auto& fboCurr = _frame % 2 == 0 ? _viewFBO1 : _viewFBO2;
	auto& fboPrev = _frame % 2 == 0 ? _viewFBO2 : _viewFBO1;

	_renderProgram->setHandle("accumTexture", fboPrev->renderTexture->getHandle());

	glBindFramebuffer(GL_FRAMEBUFFER, fboCurr->id());

	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_frame++;
	_accumFrame++;
}
void Renderer::updateCameraUniforms()
{
	_renderProgram->setFloat3("cameraPos", Camera::instance->pos());
	_renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->rot()));
}

GLFrameBuffer* Renderer::sceneViewFBO()
{
	return _frame % 2 == 0 ? _viewFBO1.get() : _viewFBO2.get();
}

void Renderer::setSamplesPerPixel(int samples)
{
	_samplesPerPixel = samples;
	_renderProgram->use();
	_renderProgram->setInt("samplesPerPixel", _samplesPerPixel);

	resetAccumulation();
}
void Renderer::setMaxRayBounces(int bounces)
{
	_maxRayBounces = bounces;
	_renderProgram->use();
	_renderProgram->setInt("maxRayBounces", _maxRayBounces);

	resetAccumulation();
}
void Renderer::resizeView(glm::ivec2 size)
{
	_viewFBO1.reset();
	_viewFBO2.reset();
	_viewFBO1 = make_unique<GLFrameBuffer>(size);
	_viewFBO2 = make_unique<GLFrameBuffer>(size);

	_renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setViewSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);

	resetAccumulation();
}

void Renderer::resetAccumulation()
{
	_accumFrame = 0;
}
