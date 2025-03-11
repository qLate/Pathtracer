#include "Program.h"

#include <SDL.h>

#include "BufferController.h"
#include "BVH.h"
#include "Debug.h"
#include "Graphical.h"
#include "ImGUIHandler.h"
#include "Input.h"
#include "MyTime.h"
#include "SDLHandler.h"
#include "Scene.h"
#include "Renderer.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	Program::init();

	Program::loop();

	Program::quit();
	return 0;
}

void Program::init()
{
	static long long dur;

	SDLHandler::init();
	Renderer::init();

	// Setup scene
	dur = Utils::measureCallTime(SceneSetup::setupScene);
	Debug::log("Scene setup in " + std::to_string(dur) + "ms");

	// Build BVH
	dur = Utils::measureCallTime(BVHBuilder::initBVH);
	Debug::log("BVH tree built in " + std::to_string(dur) + "ms");

	// Update buffers
	dur = Utils::measureCallTime(BufferController::updateAllBuffers);
	Debug::log("Buffers updated in " + std::to_string(dur) + "ms");
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

		Renderer::render();
		ImGUIHandler::update();

		SDL_GL_SwapWindow(SDLHandler::window);
		if (!SDLHandler::updateEvents()) break;
	}
}

void Program::quit()
{
	SDLHandler::quit();
	Renderer::uninit();
}
