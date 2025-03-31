#pragma once

#include <SDL2/include/SDL.h>

#include "glm/vec2.hpp"

class SDLHandler
{
	inline static SDL_Window* _window;
	inline static SDL_Event _event;
	inline static SDL_GLContext _context;

	inline static bool _isFullscreen = false;
	inline static bool _isNavigatingScene = false;

	inline static float _lastUpdateTime = 0;

	static void init();
	static void initOpenGL();

	static void update();
	static void updateLimitFPS();

	static void swapBuffers();

	static void quit();

public:
	static SDL_Window* window() { return _window; }
	static SDL_GLContext context() { return _context; }
	static bool isFullscreen() { return _isFullscreen; }
	static bool isNavigatingScene() { return _isNavigatingScene; }
	static bool isWindowMinimized();

	static void setFullscreen(bool fullscreen);
	static void setNavigatingScene(bool navigating);

	static void setWindowSize(glm::ivec2 size);

	friend class Program;
};
