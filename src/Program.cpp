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
	TimeMeasurerGL tm(0, false);

	SDLHandler::init();
	tm.printElapsedFromLast("SDL init in ");

	Renderer::init();
	tm.printElapsedFromLast("Renderer init in ");

	Physics::init();
	tm.printElapsedFromLast("Physics init in ");

	BufferController::init();
	tm.printElapsedFromLast("BufferController init in ");

	BVH::init();
	tm.printElapsedFromLast("BVH init in ");

	Debug::log("--------------------------------");
	SceneSetup::setupScene();
	tm.printElapsedFromLast("Scene setup in ");

	BufferController::initBuffers();
	tm.printElapsedFromLast("Buffers init in ");

	BVH::buildBVH();
	tm.printElapsed("Total init in ");
}

void Program::loop()
{
	while (!doQuit)
	{
		Time::update();
		Input::update();
		SDLHandler::update();
		Tweener::update();

		BufferController::checkIfBufferUpdateRequired();

		Renderer::render();
		ImGUIHandler::draw();

		SDL_GL_SwapWindow(SDLHandler::window());
	}
}

void Program::quit()
{
	SDLHandler::quit();
}
