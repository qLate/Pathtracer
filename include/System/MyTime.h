#pragma once

class Time
{
	static constexpr float MAX_DELTA_TIME = 1.0f / 12.0f;

	inline static float _time = 0;
	inline static float _deltaTime = 0;

	static void update();

public:
	float static time() { return _time; }
	float static deltaTime() { return _deltaTime; }

	friend class Program;
};
