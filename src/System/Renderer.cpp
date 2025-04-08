#include "Renderer.h"

#include "BufferController.h"
#include "Camera.h"
#include "GLObject.h"
#include "ImGuiHandler.h"
#include "Material.h"
#include "SDLHandler.h"

void Renderer::init()
{
	_renderProgram = make_unique<DefaultShaderProgram<RaytraceShader>>("shaders/common/pathtracer.vert", "shaders/pathtracer.frag");
	_renderProgram->use();

	setSPP(_samplesPerPixel);
	setMaxRayBounces(_maxRayBounces);
	setMisSampleBrdf(_misSampleBrdf);
	setMisSampleLight(_misSampleLight);
	resizeView(ImGuiHandler::INIT_RENDER_SIZE);
}

void Renderer::render()
{
	if (_limitSamples && _totalSamples >= _maxAccumSamples || SDLHandler::isWindowMinimized()) return;
	_renderProgram->use();

	updateCameraUniforms();
	BufferController::bindBuffers();

	_renderProgram->setInt("frame", _frame);
	_renderProgram->setInt("sampleFrame", _sampleFrame);
	#ifndef BENCHMARK_BUILD
	_renderProgram->setHandle("accumMeanTexture", _accumMeanTex->getHandle());
	_renderProgram->setHandle("accumSqrTexture", _accumSqrTex->getHandle());
	#endif

	glBindVertexArray(_renderProgram->fragShader()->vaoScreen()->id());
	TimeMeasurerGL tm;
	int n = _renderOneByOne ? 1 : _samplesPerPixel;
	for (int i = 0; i < n; i++)
	{
		_renderProgram->setInt("totalSamples", _totalSamples++);

		glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	_renderTime = tm.elapsedFromLast();
	glBindVertexArray(0);

	_frame++;
	_sampleFrame++;
}
void Renderer::updateCameraUniforms()
{
	_renderProgram->setFloat3("cameraPos", Camera::instance->pos());
	_renderProgram->setMatrix4X4("cameraRotMat", mat4_cast(Camera::instance->rot()));
}

float Renderer::computeSampleVariance()
{
	#ifdef BENCHMARK_BUILD
	return 0;
	#else
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
	#endif
}

void Renderer::setLimitSamples(bool limit)
{
	_limitSamples = limit;
	_renderProgram->use();
	_renderProgram->setInt("limitSamples", _limitSamples);

	if (limit && _totalSamples > _maxAccumSamples)
		resetSamples();
}
void Renderer::setSPP(int samples)
{
	_samplesPerPixel = samples;
	_renderProgram->use();
	_renderProgram->setInt("samplesPerPixel", _samplesPerPixel);

	resetSamples();
}
void Renderer::setMaxAccumSamples(int maxAccumSamples)
{
	auto prevMaxAccumSamples = _maxAccumSamples;
	_maxAccumSamples = maxAccumSamples;
	_renderProgram->use();

	if (prevMaxAccumSamples > maxAccumSamples)
		resetSamples();
}
void Renderer::setMaxRayBounces(int bounces)
{
	_maxRayBounces = bounces;
	_renderProgram->use();
	_renderProgram->setInt("maxRayBounces", _maxRayBounces);

	resetSamples();
}
void Renderer::setMisSampleBrdf(bool doSample)
{
	_misSampleBrdf = doSample;

	_renderProgram->use();
	_renderProgram->setBool("misSampleBrdf", doSample);

	resetSamples();
}
void Renderer::setMisSampleLight(bool doSample)
{
	_misSampleLight = doSample;

	_renderProgram->use();
	_renderProgram->setBool("misSampleLight", doSample);

	resetSamples();
}

void Renderer::resizeView(glm::ivec2 size)
{
	resizeTextures(size);

	_renderProgram->setFloat2("pixelSize", size);
	if (Camera::instance)
		Camera::instance->setRatio(size.x / (float)size.y);
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
	glBindFramebuffer(GL_FRAMEBUFFER, _viewFBO->id());

	#ifndef BENCHMARK_BUILD
	_accumMeanTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);
	_accumSqrTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);
	_varianceTex = make_unique<GLTexture2D>(size.x, size.y, nullptr, GL_RGB, GL_RGB32F, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _accumMeanTex->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _accumSqrTex->id(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _varianceTex->id(), 0);
	glDrawBuffers(4, DRAW_BUFFERS);
	#endif
}

void Renderer::resetSamples()
{
	_sampleFrame = 0;
	_totalSamples = 0;
	_renderTime = 0;
}
