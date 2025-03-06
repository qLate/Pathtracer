#pragma once

class Logger
{
	inline static int lastFPSPrintTime = 0;

public:

	static void updatePrintFPS();
};
