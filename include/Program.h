#pragma once

#include "SDL_stdinc.h"

class Program
{
public:
	static constexpr float FPS_LIMIT = 1000;

	inline static bool doQuit = false;

	static void init();

	static void loop();

	static void quit();
};
