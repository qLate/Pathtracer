#pragma once

#include <chrono>

#undef APIENTRY
#include "rapidobj.hpp"
#include "SDL_timer.h"
#include "Debug.h"
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"
#include <windows.h>

template <typename T> using UPtr = std::unique_ptr<T>;

template <typename T> using SPtr = std::shared_ptr<T>;

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

	template <typename T> static bool hasFlag(T flags, T flag);

	static float computeMSE(const std::vector<glm::vec3>& rendered, const std::vector<glm::vec3>& reference);

	static void copyToClipboard(const std::string& text);
};

template <typename T> bool Utils::hasFlag(T flags, T flag)
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

	float elapsed();
	float elapsedFromLast();
	float measureSum();

	void printElapsed(const std::string& msg = "");
	void printElapsedFromLast(const std::string& msg = "");
};

class TimeMeasurerGL
{
	TimeMeasurer tm;

public:
	TimeMeasurerGL(int decimals = 1, bool doFinish = true);

	float elapsed();
	float elapsedFromLast();
	void reset();

	void printElapsed(const std::string& msg = "");
	void printElapsedFromLast(const std::string& msg = "");
};

template <class T> struct Flags
{
	uint32_t value;

	Flags() : value(0) {}
	Flags(T val) : value(1u << static_cast<uint32_t>(val)) {}
	Flags(uint32_t val) : value(val) {}
	Flags(const Flags& other) : value(other.value) {}

	Flags& operator =(const Flags& other)
	{
		value = other.value;
		return *this;
	}

	void set(T val) { value |= (1u << static_cast<uint32_t>(val)); }
	void clear(T val) { value &= ~(1u << static_cast<uint32_t>(val)); }
	void toggle(T val) { value ^= (1u << static_cast<uint32_t>(val)); }

	void setAll() { value = 0xFFFFFFFFu; }
	void clearAll() { value = 0u; }
	void toggleAll() { value = ~value; }

	bool has(T val) const { return (value & 1u << static_cast<uint32_t>(val)) != 0u; }
};

template <class T> Flags<T> operator |(Flags<T> lhs, Flags<T> rhs) { return Flags<T>(lhs.val | rhs.val); }
template <class T> Flags<T> operator &(Flags<T> lhs, Flags<T> rhs) { return Flags<T>(lhs.val & rhs.val); }
template <class T> Flags<T> operator ^(Flags<T> lhs, Flags<T> rhs) { return Flags<T>(lhs.val ^ rhs.val); }
template <class T> Flags<T> operator ~(Flags<T> rhs) { return Flags<T>(~rhs.val); }

template <class T> Flags<T> operator |(Flags<T> lhs, T rhs) { return lhs | Flags<T>(rhs); }
template <class T> Flags<T> operator &(Flags<T> lhs, T rhs) { return lhs & Flags<T>(rhs); }
template <class T> Flags<T> operator ^(Flags<T> lhs, T rhs) { return lhs ^ Flags<T>(rhs); }

template <class T> Flags<T> operator |(T lhs, T rhs) { return Flags<T>(lhs) | Flags<T>(rhs); }
