#include "SDLHandler.h"

#include "Debug.h"
#include "glad.h"
#include "GLObject.h"
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	auto requiredSize = ImGUIHandler::calculateInitialFullWindowSize();
	window = SDL_CreateWindow("Pathtracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, requiredSize.x, requiredSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	context = SDL_GL_CreateContext(window);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	initOpenGL();
	ImGUIHandler::initialize();

	SDL_GL_SetSwapInterval(0);
}
void SDLHandler::initOpenGL()
{
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
		Debug::log("OpenGL Debug Message: ", message);
	}, nullptr);

	Pathtracer::sceneViewFBO = new GLFrameBuffer(ImGUIHandler::INIT_RENDER_SIZE.x, ImGUIHandler::INIT_RENDER_SIZE.y);
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
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void SDLHandler::setWindowSize(glm::ivec2 size)
{
	SDL_SetWindowSize(window, size.x, size.y);
}
