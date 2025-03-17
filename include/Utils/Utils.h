#pragma once

#include <chrono>

#include "rapidobj.hpp"
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

	static std::string toBinary(int n, int bits = -1)
	{
		std::string r;
		while (n != 0 || bits > 0)
		{
			r.insert(0, n % 2 == 0 ? "0" : "1");
			n /= 2;
			bits--;
		}
		return r;
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

template <typename T = std::chrono::milliseconds>
class TimeMeasurer
{
	std::chrono::high_resolution_clock::time_point _start;
	std::chrono::high_resolution_clock::time_point _lastMeasure;
	long long timeSum = 0;

public:
	TimeMeasurer()
	{
		_start = std::chrono::high_resolution_clock::now();
		_lastMeasure = _start;
	}

	void start()
	{
		_start = std::chrono::high_resolution_clock::now();
	}
	void stop()
	{
		auto curr = std::chrono::high_resolution_clock::now();
		timeSum += std::chrono::duration_cast<T>(curr - _start).count();
	}
	void reset()
	{
		timeSum = 0;
	}

	long long measure()
	{
		_lastMeasure = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<T>(_lastMeasure - _start).count();
	}
	long long measureFromLast()
	{
		auto curr = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<T>(curr - _lastMeasure).count();

		_lastMeasure = curr;
		return dur;
	}
	long long measureSum()
	{
		auto curr = std::chrono::high_resolution_clock::now();
		timeSum += std::chrono::duration_cast<T>(curr - _lastMeasure).count();
		_lastMeasure = curr;

		return timeSum;
	}
};
