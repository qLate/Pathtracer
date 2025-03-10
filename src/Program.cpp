#include "Program.h"

#include <SDL.h>

#include "BufferController.h"
#include "BVH.h"
#include "Graphical.h"
#include "ImGUIHandler.h"
#include "Input.h"
#include "MyTime.h"
#include "SDLHandler.h"
#include "Scene.h"
#include "Pathtracer.h"

int main(int argc, char* argv[])
{
	Program::initialize();

	Program::loop();

	Program::quit();
	return 0;
}

void Program::initialize()
{
	SDLHandler::initialize();
	Pathtracer::initTraceShader();
	SceneSetup::setupScene();
	BVHBuilder::initializeBVH();
	BufferController::updateAllBuffers();
}

void Program::loop()
{
	auto lastUpdateTime = SDL_GetTicks();
	while (true)
	{
		// Limit FPS
		double delta = SDL_GetTicks() - lastUpdateTime;
		if (delta < 1000 / FPS_LIMIT) continue;
		lastUpdateTime += delta;
		// Limit FPS

		Time::updateTime();
		Input::updateInput();

		Pathtracer::traceScene();
		ImGUIHandler::update();

		SDL_GL_SwapWindow(SDLHandler::window);
		if (!SDLHandler::updateEvents()) break;

	}
}

void Program::quit()
{
	SDLHandler::quit();
	Pathtracer::uninit();
}
