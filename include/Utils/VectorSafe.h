#pragma once

#include <vector>
#include <functional>

template <typename T> class VectorSafe : public std::vector<T>
{
	std::vector<T> _toRemove {};

public:
	void erase_delayed(const T& t)
	{
		_toRemove.push_back(t);
	}
	void erase_delayed_if(std::function<bool(const T&)> predicate)
	{
		for (const T& t : *this)
		{
			if (!predicate(t)) continue;
			_toRemove.push_back(t);
			return;
		}
	}

	void apply_changes()
	{
		for (const T& t : _toRemove)
			std::erase(*this, t);
		_toRemove.clear();
	}
};

template <typename T> class VectorSafeRef : public std::vector<T>
{
	std::vector<std::reference_wrapper<const T>> _toRemove{};

public:
	void erase_delayed(const T& t)
	{
		_toRemove.push_back(t);
	}
	void erase_delayed_if(std::function<bool(const T&)> predicate)
	{
		for (const T& t : *this)
		{
			if (!predicate(t)) continue;
			_toRemove.push_back(t);
			return;
		}
	}

	void apply_changes()
	{
		for (const T& t : _toRemove)
			std::erase(*this, t);
		_toRemove.clear();
	}
};
