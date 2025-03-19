#pragma once

#include <SDL2/include/SDL.h>

#include "glm/vec2.hpp"

class SDLHandler
{
	inline static SDL_Window* _window;
	inline static SDL_Event _event;
	inline static SDL_GLContext _context;

	inline static bool _isMouseAttachedToScene = false;
	inline static bool _isFullscreen = false;

	inline static float _lastUpdateTime = 0;

	static void init();
	static void initOpenGL();

	static void update();
	static void updateLimitFPS();

	static void quit();

public:
	static SDL_Window* window() { return _window; }
	static SDL_GLContext context() { return _context; }
	static bool isMouseAttachedToScene() { return _isMouseAttachedToScene; }
	static bool isFullscreen() { return _isFullscreen; }

	static void setFullscreen(bool fullscreen);
	static void setAttachMouseToScene(bool attached);

	static void setWindowSize(glm::ivec2 size);

	friend class Program;
};
