#pragma once

class Logger
{
	inline static int frameCount = 0;
	inline static int lastFrameTime = 0;
public:
	inline static int fps = 0;

	static void logStats();
	static void updateFPSCounter();
};