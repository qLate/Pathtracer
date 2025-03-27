#include "Renderer.h"

#include "BufferController.h"
#include "Camera.h"
#include "GLObject.h"
#include "ImGuiHandler.h"
#include "Material.h"
#include "WindowDrawer.h"

void Renderer::init()
{
	_renderProgram = make_unique<DefaultShaderProgram<RaytraceShader>>("shaders/common/pathtracer.vert", "shaders/pathtracer.frag");
	_renderProgram->use();
	_renderProgram->setFloat2("pixelSize", ImGuiHandler::INIT_RENDER_SIZE);
	setSamplesPerPixel(_samplesPerPixel);
	setMaxRayBounces(_maxRayBounces);

	resizeTextures(ImGuiHandler::INIT_RENDER_SIZE);
}

void Renderer::render()
{
	if (_renderOnce && _accumFrame > 0) return;

	_renderProgram->use();
	_renderProgram->setInt("frame", _frame);
	_renderProgram->setInt("accumFrame", _accumFrame);

	updateCameraUniforms();

	auto& fboCurr = _frame % 2 == 0 ? _viewFBO1 : _viewFBO2;
	auto& fboPrev = _frame % 2 == 0 ? _viewFBO2 : _viewFBO1;

	_renderProgram->setHandle("accumTexture", fboPrev->renderTexture()->getHandle());
	_renderProgram->setHandle("accumSqrTexture", _accumSqrTex->getHandle());

	glBindFramebuffer(GL_FRAMEBUFFER, fboCurr->id());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboCurr->renderTexture()->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _accumSqrTex->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _varianceTex->id(), 0);
	glDrawBuffers(3, DRAW_BUFFERS);

	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	if (_renderOnce && _frame >= 2)
	{
		TimeMeasurerGL tm {};
		glDrawArrays(GL_TRIANGLES, 0, 6);
		tm.printElapsed("Render time: ");
	}
	else
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

float Renderer::computeAccumVariance()
{
	auto varianceData = _varianceTex->readData<glm::vec3>();

	float varianceSum = 0;
#pragma omp parallel for reduction(+:varianceSum)
	for (int i = 0; i < varianceData.size(); i++)
	{
		glm::vec3& pixelVariance = varianceData[i];
		varianceSum += (pixelVariance.r + pixelVariance.g + pixelVariance.b) / 3;
	}
	return varianceSum / varianceData.size();
}

GLFrameBuffer* Renderer::sceneViewFBO()
{
	return _frame % 2 == 0 || _renderOnce ? _viewFBO1.get() : _viewFBO2.get();
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
	resizeTextures(size);

	_renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setViewSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);

	resetAccumulation();
}
void Renderer::resizeTextures(glm::ivec2 size)
{
	_viewFBO1.reset();
	_viewFBO2.reset();
	_accumSqrTex.reset();
	_varianceTex.reset();

	_viewFBO1 = make_unique<GLFrameBuffer>(size);
	_viewFBO2 = make_unique<GLFrameBuffer>(size);
	_accumSqrTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F);
	_varianceTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F);
}

void Renderer::resetAccumulation()
{
	_accumFrame = 0;
}
