#pragma once

#include <SDL2/include/SDL.h>

class SDLHandler
{
public:
	inline static SDL_Renderer* renderer;
	inline static SDL_Window* window;
	inline static SDL_Event event;
	inline static SDL_GLContext context;

	inline static bool windowFocused = true;
	inline static bool isFullscreen = false;

	static void initialize(int width, int height);
	static bool update();
	static void quit();
};
