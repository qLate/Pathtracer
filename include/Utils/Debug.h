#pragma once
#include <iostream>
#include <sstream>

enum class LogType : std::uint8_t
{
	Default = 1,
	Input = 2,
	FunctionCalls = 4,
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
public:
	inline static bool loggingEnabled = true;
	inline static auto enabledLogTypes = LogType::Default;

	inline static int depth = 0;
	inline static bool preventSpam = true;
	inline static std::string last;

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
};
