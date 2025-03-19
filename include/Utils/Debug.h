#pragma once

#include <iostream>
#include <sstream>

enum class LogType : std::uint8_t
{
	Default = 1,
	Input = 2,
};

inline LogType operator|(LogType a, LogType b)
{
	return (LogType)((int)a | (int)b);
}
inline bool hasFlag(LogType flags, LogType flag)
{
	return ((int)flags & (int)flag) != 0;
}

class Debug
{
	inline static int _depth = 0;
	inline static std::string _last;

public:
	inline static bool enableLogging = true;
	inline static auto enabledLogTypes = LogType::Default;
	inline static bool preventSpam = true;

	template <typename... P>
	static void log(const P&... params)
	{
		std::stringstream stream;
		(stream << ... << params);
		std::cout << stream.str() << '\n';
	}

	template <typename... P>
	static void log(LogType logType, const P&... params)
	{
		if (!hasFlag(enabledLogTypes, logType)) return;
		log<P...>(params);
	}

	template <typename... P>
	static void logError(const P&... params)
	{
		std::stringstream stream;
		(stream << ... << params);
		std::cerr << stream.str() << '\n';
	}
};
