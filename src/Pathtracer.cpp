#include "Pathtracer.h"

#include <iostream>
#include <SDL.h>

#include "BufferController.h"
#include "BVH.h"
#include "Camera.h"
#include "Graphical.h"
#include "Input.h"
#include "Logger.h"
#include "MyTime.h"
#include "SDLHandler.h"
#include "Triangle.h"
#include "TraceShader.h"
#include "Scene.h"

int main(int argc, char* argv[])
{
	Pathtracer::initialize(640 * 2, 360 * 2);

	Pathtracer::loop();

	Pathtracer::quit();
	return 0;
}

void Pathtracer::initialize(int width, int height)
{
	Pathtracer::width = width;
	Pathtracer::height = height;

	SDLHandler::initialize(width, height);
	shader = new TraceShader("shaders/pathtracer.vert", "shaders/pathtracer.frag");
	shader->use();

	shader->setInt("maxRayBounce", 5);
	shader->setFloat2("pixelSize", {width, height});

	onUpdate += Time::updateTime;
	onUpdate += Input::updateInput;
	onUpdate += Logger::updateFPSCounter;
	onUpdate += Logger::logStats;

	initializeScene();
	BVHBuilder::initializeBVH();
	BufferController::initializeUniformBuffers();
}

void Pathtracer::initializeFBO(Shader*& screenShader, unsigned& fbo, unsigned& renderTexture)
{
	screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
	screenShader->use();
	screenShader->setInt("width", width);
	screenShader->setInt("height", height);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &renderTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	screenShader->use();
	glUniform1i(glGetUniformLocation(screenShader->id, "screenTexture"), 1);
}

void Pathtracer::initializeScene()
{
	SceneSetup::setupScene();
}

void Pathtracer::loop()
{
	auto currRow = 0;
	while (true)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Draw scene
		onUpdate();
		shader->use();
		glBindVertexArray(shader->vaoScreen->id);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		shader->setInt("currRow", currRow++);

		SDL_GL_SwapWindow(SDLHandler::window);
		if (!SDLHandler::update()) break;
	}
}

void Pathtracer::quit()
{
	SDLHandler::quit();
	delete shader;
}
