#pragma once

#include <functional>

template <typename... Ts> class Action
{
	std::vector<std::function<void(Ts...)>> _callbacks {};

public:
	int subscribe(const std::function<void(Ts...)>& func)
	{
		_callbacks.emplace_back(func);
		return _callbacks.size() - 1;
	}
	void unsubscribe(int id)
	{
		_callbacks.erase(id);
	}

	void operator+=(const std::function<void(Ts...)>& func)
	{
		subscribe(func);
	}

	void operator()(Ts... args) const
	{
		for (auto& func : _callbacks)
			func(args...);
	}
};
