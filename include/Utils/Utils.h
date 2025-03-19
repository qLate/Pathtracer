#pragma once

#include <chrono>

#undef APIENTRY
#include "rapidobj.hpp"
#include "SDL_timer.h"
#include "Debug.h"

template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;

using std::make_unique;
using std::make_shared;

class Utils
{
public:
	static float round(float value, int decimals);
	static int mod(int k, int n);
	static float mod(float k, float n);

	static std::string toBinary(int n, int bits = -1);
	static std::string toString(float f, int decimals = 2);

	static long long measureCallTime(void (*func)());

	template <typename T>
	static bool hasFlag(T flags, T flag);
};

template <typename T>
bool Utils::hasFlag(T flags, T flag)
{
	return ((int)flags & (int)flag) != 0;
}

class Timer
{
	float _delay;
	float _lastTriggerTime = 0.0f;

public:
	Timer(float delay) : _delay(delay) {}

	bool trigger();
};

class TimeMeasurer
{
	int _decimals;

	std::chrono::high_resolution_clock::time_point _start;
	std::chrono::high_resolution_clock::time_point _lastMeasure;
	long long timeSum = 0;

public:
	TimeMeasurer(int decimals = 1);

	void reset();

	long long elapsed();
	long long elapsedFromLast();
	long long measureSum();

	void printElapsed(const std::string& msg = "");
	void printElapsedFromLast(const std::string& msg = "");
};

class TimeMeasurerGL
{
	TimeMeasurer tm;

public:
	TimeMeasurerGL(int decimals = 1, bool doFinish = true);

	void reset();

	void printElapsed(const std::string& msg = "");
	void printElapsedFromLast(const std::string& msg = "");
};
