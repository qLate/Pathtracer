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
	SDLHandler::initialize();

	traceShaderP = new ShaderProgram<TraceShader>("shaders/default/pathtracer.vert", "shaders/pathtracer.frag");
	traceShaderP->use();
	traceShaderP->setInt("maxRayBounce", MAX_RAY_BOUNCE);
	traceShaderP->setFloat2("pixelSize", SDLHandler::W_SIZE);

	onUpdate += Time::updateTime;
	onUpdate += Input::updateInput;
	onUpdate += Logger::updatePrintFPS;

	SceneSetup::setupScene();
	BVHBuilder::initializeBVH();
	BufferController::updateAllBuffers();
}

void Pathtracer::loop()
{
	while (true)
	{
		onUpdate();

		SDLHandler::updateDrawScene();

		SDLHandler::updateImGui();

		SDL_GL_SwapWindow(SDLHandler::window);
		if (!SDLHandler::updateEvents()) break;
	}
}

void Pathtracer::quit()
{
	SDLHandler::quit();
	delete traceShaderP;
}
