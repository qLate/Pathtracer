#pragma once

class Utils
{
public:
	static float round(float value, int decimals)
	{
		float mult = pow(10, decimals);
		return std::round(value * mult) / mult;
	}
};
