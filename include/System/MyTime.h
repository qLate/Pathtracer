#pragma once

class Time
{
public:
	inline static constexpr float MAX_DELTA_TIME = 1.0f / 12.0f;

	inline static float time = 0;
	inline static float deltaTime = 0;

	static void updateTime();
};
