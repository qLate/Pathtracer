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
	SDLHandler::init();
	Renderer::init();

	TimeMeasurer tm {};

	SceneSetup::setupScene();
	Debug::log("Scene setup in " + std::to_string(tm.measureFromLast()) + "ms");

	BVHBuilder::initBVH();
	Debug::log("BVH tree built in " + std::to_string(tm.measureFromLast()) + "ms");

	BufferController::updateAllBuffers();
	Debug::log("Buffers updated in " + std::to_string(tm.measureFromLast()) + "ms");
}

void Program::loop()
{
	while (!doQuit)
	{
		Time::update();
		Input::update();
		SDLHandler::update();

		Renderer::render();
		ImGUIHandler::draw();

		SDL_GL_SwapWindow(SDLHandler::window);
	}
}

void Program::quit()
{
	SDLHandler::quit();
	Renderer::uninit();
}
