#pragma once

#include <vector>
#include <functional>

template <typename T> class VectorSafe : public std::vector<T>
{
	std::vector<T> toRemove {};

public:
	void erase_delayed(const T& t)
	{
		toRemove.push_back(t);
	}
	void erase_delayed_if(std::function<bool(const T&)> predicate)
	{
		for (const T& t : *this)
		{
			if (!predicate(t)) continue;
			toRemove.push_back(t);
			return;
		}
	}

	void apply_changes()
	{
		for (const T& t : toRemove)
			std::erase(*this, t);
		toRemove.clear();
	}
};

template <typename T> class VectorSafeRef : public std::vector<T>
{
	std::vector<std::reference_wrapper<const T>> toRemove{};

public:
	void erase_delayed(const T& t)
	{
		toRemove.push_back(t);
	}
	void erase_delayed_if(std::function<bool(const T&)> predicate)
	{
		for (const T& t : *this)
		{
			if (!predicate(t)) continue;
			toRemove.push_back(t);
			return;
		}
	}

	void apply_changes()
	{
		for (const T& t : toRemove)
			std::erase(*this, t);
		toRemove.clear();
	}
};
