#include "SDLHandler.h"

#include <iostream>

#include "glad.h"
#include "ImGUIHandler.h"
#include "imgui_impl_sdl2.h"
#include "Input.h"
#include "Pathtracer.h"

void SDLHandler::initialize()
{
	SDL_GL_LoadLibrary(nullptr);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	window = SDL_CreateWindow("Pathtracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W_WIDTH, W_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	ImGUIHandler::init();
	initOpenGL();
}
void SDLHandler::initOpenGL()
{
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Pathtracer::sceneViewFBO = new GLFrameBuffer(W_WIDTH, W_HEIGHT);
}

bool SDLHandler::updateEvents()
{
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		Input::handleSDLEvent(event);

		if (event.type == SDL_QUIT) return false;
	}
	return true;
}

void SDLHandler::quit()
{
	//delete sceneViewFBO;

	SDL_DestroyWindow(window);
	SDL_Quit();
}
