#pragma once

#include <SDL2/include/SDL.h>

#include "glm/vec2.hpp"

class SDLHandler
{
public:
	inline static SDL_Window* window;
	inline static SDL_Event event;
	inline static SDL_GLContext context;

	inline static bool sceneFocused = true;
	inline static bool isFullscreen = false;

	static void init();
	static void initOpenGL();

	static bool updateEvents();

	static void quit();

	static void setWindowSize(glm::ivec2 size);
};
