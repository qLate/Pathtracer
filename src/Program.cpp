#include "Program.h"

#include <SDL.h>

#include "BufferController.h"
#include "BVH.h"
#include "ImGUIHandler.h"
#include "Input.h"
#include "MyTime.h"
#include "Physics.h"
#include "SDLHandler.h"
#include "Scene.h"
#include "Renderer.h"
#include "Tweener.h"
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
	Physics::init();
	BufferController::init();
	BVH::init();

	TimeMeasurer tm {};

	SceneSetup::setupScene();
	tm.printElapsedFromLast("Scene setup in ");

	BufferController::writeBuffers();
	tm.printElapsedFromLast("Buffers written in ");

	BVH::buildBVH();
	tm.printElapsedFromLast("BVH tree built in ");
}

void Program::loop()
{
	while (!doQuit)
	{
		Time::update();
		Input::update();
		SDLHandler::update();
		Tweener::update();

		Renderer::render();
		ImGUIHandler::draw();

		SDL_GL_SwapWindow(SDLHandler::window);
	}
}

void Program::quit()
{
	SDLHandler::quit();
}
