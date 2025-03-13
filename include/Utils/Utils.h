#pragma once

#include <chrono>

#include "SDL_timer.h"

class Utils
{
public:
	static float round(float value, int decimals)
	{
		float mult = powf(10, decimals);
		return std::round(value * mult) / mult;
	}

	static long long measureCallTime(void (*func)())
	{
		auto start = std::chrono::high_resolution_clock::now();
		func();
		auto end = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}

	static int mod(int k, int n)
	{
		return (k % n + n) % n;
	}
	static float mod(float k, float n)
	{
		return fmod(fmod(k, n) + n, n);
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

class TimeMeasurer
{
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point lastMeasure;

public:
	TimeMeasurer()
	{
		start = std::chrono::high_resolution_clock::now();
		lastMeasure = start;
	}

	long long measure()
	{
		lastMeasure = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(lastMeasure - start).count();
	}
	long long measureFromLast()
	{
		auto curr = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(curr - lastMeasure).count();

		lastMeasure = curr;
		return dur;
	}
};
