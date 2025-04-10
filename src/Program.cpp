#include "Program.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <SDL.h>

#include "BufferController.h"
#include "BVH.h"
#include "ImGuiHandler.h"
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

	ImGuiHandler::init();
	tm.printElapsedFromLast("ImGui init in ");

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
	tm.printElapsedFromLast("BVH built in ");

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
		ImGuiHandler::draw();

		SDLHandler::swapBuffers();
	}
}

void Program::quit()
{
	SDLHandler::quit();
}
