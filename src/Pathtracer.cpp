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
	Pathtracer::initialize();

	Pathtracer::loop();

	Pathtracer::quit();
	return 0;
}

void Pathtracer::initialize()
{
	SDLHandler::initialize(W_WIDTH, W_HEIGHT);

	shaderProgram = new ShaderProgram<TraceShader>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	shaderProgram->use();
	shaderProgram->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	shaderProgram->setFloat2("pixelSize", W_SIZE);

	onUpdate += Time::updateTime;
	onUpdate += Input::updateInput;
	onUpdate += Logger::updateFPSCounter;

	initializeScene();
	BVHBuilder::initializeBVH();
	BufferController::updateAllBuffers();
}

void Pathtracer::initializeScene()
{
	SceneSetup::setupScene();
}

void Pathtracer::loop()
{
	while (true)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Draw scene
		onUpdate();

		shaderProgram->use();
		glBindVertexArray(shaderProgram->fragShader->vaoScreen->id);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(SDLHandler::window);
		if (!SDLHandler::update()) break;
	}
}

void Pathtracer::quit()
{
	SDLHandler::quit();
	delete shaderProgram;
}
