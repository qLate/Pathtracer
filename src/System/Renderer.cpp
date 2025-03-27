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
	if (_limitSamples && _currSamples >= _samplesPerPixel) return;
	_renderProgram->use();
	_renderProgram->setInt("frame", _frame);
	_renderProgram->setInt("currSamples", _currSamples);

	updateCameraUniforms();

	_renderProgram->setHandle("accumMeanTexture", _accumMeanTex->getHandle());
	_renderProgram->setHandle("accumSqrTexture", _accumSqrTex->getHandle());

	glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());
	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_frame++;
	_currSamples++;
}
void Renderer::updateCameraUniforms()
{
	_renderProgram->setFloat3("cameraPos", Camera::instance->pos());
	_renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->rot()));
}

float Renderer::computeSampleVariance()
{
	auto meanData = _accumMeanTex->readData<glm::vec3>();
	auto sqrData = _accumSqrTex->readData<glm::vec3>();
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

void Renderer::setLimitSamples(bool limit)
{
	_limitSamples = limit;
	_renderProgram->use();
	_renderProgram->setInt("limitSamples", _limitSamples);

	resetSamples();
}
void Renderer::setSamplesPerPixel(int samples)
{
	_samplesPerPixel = samples;
	_renderProgram->use();
	_renderProgram->setInt("samplesPerPixel", _samplesPerPixel);

	resetSamples();
}
void Renderer::setMaxRayBounces(int bounces)
{
	_maxRayBounces = bounces;
	_renderProgram->use();
	_renderProgram->setInt("maxRayBounces", _maxRayBounces);

	resetSamples();
}

void Renderer::resizeView(glm::ivec2 size)
{
	resizeTextures(size);

	_renderProgram->setFloat2("pixelSize", size);
	Camera::instance->setViewSize({size.x / (float)size.y, 1});
	glViewport(0, 0, size.x, size.y);

	resetSamples();
}
void Renderer::resizeTextures(glm::ivec2 size)
{
	_viewFBO.reset();
	_accumMeanTex.reset();
	_accumSqrTex.reset();
	_varianceTex.reset();

	_viewFBO = make_unique<GLFrameBuffer>(size);
	_accumMeanTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);
	_accumSqrTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);
	_varianceTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _accumMeanTex->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _accumSqrTex->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _varianceTex->id(), 0);
	glDrawBuffers(4, DRAW_BUFFERS);
}

void Renderer::resetSamples()
{
	_currSamples = 0;
}
