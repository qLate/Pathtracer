#pragma once

#include <SDL2/include/SDL.h>

#include "imgui.h"
#include "glm/vec2.hpp"

class SDLHandler
{
public:
	inline static constexpr int W_WIDTH = 640 * 2;
	inline static constexpr int W_HEIGHT = 360 * 2;
	inline static constexpr glm::ivec2 W_SIZE = {W_WIDTH, W_HEIGHT};
	inline static constexpr glm::vec2 W_RATIO = {W_WIDTH / (float)W_HEIGHT, 1};

	inline static SDL_Window* window;
	inline static SDL_Event event;
	inline static SDL_GLContext context;

	inline static bool windowFocused = true;
	inline static bool isFullscreen = false;

	static void initialize();
	static void initOpenGL();

	static bool updateEvents();

	static void quit();
};
