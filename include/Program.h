#pragma once

#include "Action.h"

class Program
{
	static constexpr float FPS_LIMIT = 500;

public:
	inline static Action onUpdate {};

	static void initialize();

	static void loop();

	static void quit();
};
