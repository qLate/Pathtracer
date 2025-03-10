#pragma once
#include <functional>

#include "SDL_timer.h"

class Utils
{
public:
	static float round(float value, int decimals)
	{
		float mult = powf(10, decimals);
		return std::round(value * mult) / mult;
	}
};

class Timer
{
	float delay;
	float lastTriggerTime = 0.0f;

public:
	Timer(float delay) : delay(delay) {}

	bool trigger()
	{
		float currTime = SDL_GetTicks();
		if (currTime >= lastTriggerTime + delay)
		{
			lastTriggerTime = currTime;
			return true;
		}
		return false;
	}
};
