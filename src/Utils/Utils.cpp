#include "Utils.h"

#include "glad.h"
#include "MyMath.h"

long long Utils::measureCallTime(void (*func)())
{
	auto start = std::chrono::high_resolution_clock::now();
	func();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

std::string Utils::toBinary(int n, int bits)
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
std::string Utils::toString(float f, int decimals)
{
	return std::to_string(Math::round(f, decimals));
}

float Utils::computeMSE(const std::vector<glm::vec3>& rendered, const std::vector<glm::vec3>& reference)
{
	assert(rendered.size() == reference.size());
	float mse = 0.0f;
	for (size_t i = 0; i < rendered.size(); ++i)
	{
		glm::vec3 diff = rendered[i] - reference[i];
		mse += dot(diff, diff);
	}
	mse /= (float)rendered.size();
	return mse;
}

void Utils::copyToClipboard(const std::string& text)
{
	if (OpenClipboard(nullptr))
	{
		EmptyClipboard();

		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
		memcpy(GlobalLock(hMem), text.c_str(), text.size() + 1);
		GlobalUnlock(hMem);

		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}
}

std::string Utils::toString(const glm::vec3& v, int precision)
{
	auto mult = pow(10, precision);
	return "(" + std::to_string(glm::round(v.x * mult) / mult) + ", " + std::to_string(glm::round(v.y * mult) / mult) + ", " + std::to_string(glm::round(v.z * mult) / mult) + ")";
}
std::string Utils::toString(const glm::vec2& v, int precision)
{
	auto mult = pow(10, precision);
	return "(" + std::to_string(glm::round(v.x * mult) / mult) + ", " + std::to_string(glm::round(v.y * mult) / mult) + ")";
}

bool Timer::trigger()
{
	float currTime = SDL_GetTicks();
	if (currTime >= _lastTriggerTime + _delay)
	{
		_lastTriggerTime = currTime;
		return true;
	}
	return false;
}

TimeMeasurer::TimeMeasurer(int decimals): _decimals(decimals)
{
	reset();
}
void TimeMeasurer::reset()
{
	_start = std::chrono::high_resolution_clock::now();
	_lastMeasure = _start;
	timeSum = 0;
}
float TimeMeasurer::elapsed()
{
	_lastMeasure = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(_lastMeasure - _start).count() / 1000.0f;
}

float TimeMeasurer::elapsedFromLast()
{
	auto curr = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(curr - _lastMeasure).count();

	_lastMeasure = curr;
	return dur / 1000.0f;
}
float TimeMeasurer::measureSum()
{
	auto curr = std::chrono::high_resolution_clock::now();
	timeSum += std::chrono::duration_cast<std::chrono::microseconds>(curr - _lastMeasure).count();
	_lastMeasure = curr;

	return timeSum / 1000.0f;
}

void TimeMeasurer::printElapsed(const std::string& msg)
{
	Debug::log(msg, std::format("{}", Math::round(elapsed(), _decimals)), "ms");
}
void TimeMeasurer::printElapsedFromLast(const std::string& msg)
{
	Debug::log(msg, std::format("{}", Math::round(elapsedFromLast(), _decimals)), "ms");
}

TimeMeasurerGL::TimeMeasurerGL(int decimals, bool doFinish): tm(decimals)
{
	#ifndef BENCHMARK_BUILD
	if (doFinish)
		glFinish();
	#endif
	tm.reset();
}

float TimeMeasurerGL::elapsed()
{
	#ifndef BENCHMARK_BUILD
	glFinish();
	#endif
	return tm.elapsed();
}
float TimeMeasurerGL::elapsedFromLast()
{
	#ifndef BENCHMARK_BUILD
	glFinish();
	#endif
	return tm.elapsedFromLast();
}
void TimeMeasurerGL::reset()
{
	#ifndef BENCHMARK_BUILD
	glFinish();
	#endif
	tm.reset();
}

void TimeMeasurerGL::printElapsed(const std::string& msg)
{
	#ifndef BENCHMARK_BUILD
	glFinish();
	#endif
	tm.printElapsed(msg);
}
void TimeMeasurerGL::printElapsedFromLast(const std::string& msg)
{
	#ifndef BENCHMARK_BUILD
	glFinish();
	#endif
	tm.printElapsedFromLast(msg);
}
