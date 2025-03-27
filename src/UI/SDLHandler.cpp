#include "SDLHandler.h"

#include <algorithm>

#include "Debug.h"
#include "glad.h"
#include "GLObject.h"
#include "ImGuiHandler.h"
#include "imgui_impl_sdl2.h"
#include "Input.h"
#include "Program.h"
#include "Renderer.h"

void SDLHandler::init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_LoadLibrary(nullptr);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	auto initialSize = ImGuiHandler::INIT_FULL_WINDOW_SIZE;
	_window = SDL_CreateWindow("Pathtracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, initialSize.x, initialSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	_context = SDL_GL_CreateContext(_window);
	//SDL_SetRelativeMouseMode(SDL_TRUE);

	initOpenGL();

	SDL_GL_SetSwapInterval(0);
}

void SDLHandler::initOpenGL()
{
	gladLoaderLoadGL();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEBUG_OUTPUT);
	glad_glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
		Debug::log("OpenGL msg: ", message);
	}, nullptr);

	Shader::addInclude("shaders/common/common.glsl");
	Shader::addInclude("shaders/common/utils.glsl");
	Shader::addInclude("shaders/intersection.glsl");
	Shader::addInclude("shaders/light.glsl");
}

void SDLHandler::update()
{
	while (SDL_PollEvent(&_event))
	{
		ImGui_ImplSDL2_ProcessEvent(&_event);
		Input::handleSDLEvent(_event);

		if (_event.type == SDL_QUIT)
		{
			Program::doQuit = true;
			return;
		}
	}

	updateLimitFPS();
}

void SDLHandler::updateLimitFPS()
{
	Uint32 delta = SDL_GetTicks() - _lastUpdateTime;
	float targetDelta = 1000 / Program::FPS_LIMIT;
	while (delta < targetDelta)
		delta = SDL_GetTicks() - _lastUpdateTime;

	_lastUpdateTime += delta;
	_lastUpdateTime = std::max(_lastUpdateTime, SDL_GetTicks() - targetDelta);
}

void SDLHandler::swapBuffers()
{
	SDL_GL_SwapWindow(_window);
}

void SDLHandler::quit()
{
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void SDLHandler::setFullscreen(bool fullscreen)
{
	_isFullscreen = fullscreen;

	if (_isFullscreen)
		SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(_window, 0);
}
void SDLHandler::setNavigatingScene(bool navigating)
{
	_isNavigatingScene = navigating;
	SDL_SetRelativeMouseMode(navigating ? SDL_TRUE : SDL_FALSE);
}
void SDLHandler::setWindowSize(glm::ivec2 size)
{
	SDL_SetWindowSize(_window, size.x, size.y);
}
