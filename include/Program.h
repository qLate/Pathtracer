#pragma once

#include "Action.h"

class Program
{
	static constexpr float FPS_LIMIT = 500;

public:
	static void init();

	static void loop();

	static void quit();
};
