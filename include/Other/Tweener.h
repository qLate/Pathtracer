#pragma once

#include <functional>

#include "Color.h"
#include "MyMath.h"
#include "VectorSafe.h"
#include "glm/vec2.hpp"

class Object;
class Tween;

class Tweener
{
	inline static VectorSafe<Tween*> _tweens;

	static void update();

	template <typename T> static Tween* valueTo(T startValue, T endValue, float time, const std::function<void(T)>& setter);

public:
	static Tween* delayedCall(const std::function<void()>& function, float delay);

	static Tween* floatTo(float startValue, float endValue, float time, const std::function<void(float)>& setter);
	static Tween* vec2To(const glm::vec2& startValue, const glm::vec2& endValue, float time, const std::function<void(glm::vec2)>& setter);
	static Tween* colorTo(const Color& startValue, const Color& endValue, float time, const std::function<void(Color)>& setter);

	friend class Tween;
	friend class Program;
};

class Tween
{
protected:
	float _time;
	float _elapsed = 0;
	float _delay = 0;
	EaseType _ease = EaseType::InOutQuad;

	Object* _target = nullptr;

	std::function<void()> _onComplete = nullptr;

	Tween(float time);
	virtual ~Tween() = default;

	virtual void update(float deltaTime);
	virtual void finish();

public:
	void kill();

	Tween* setEase(EaseType ease);
	Tween* setDelay(float delay);
	Tween* onComplete(const std::function<void()>& onComplete);
	Tween* setTarget(Object* target);

	friend class Tweener;
	friend class Object;
};

class DelayedCall : public Tween
{
	std::function<void()> _function;

	DelayedCall(const std::function<void()>& function, float delay);

public:
	void finish() override;

	friend class Tweener;
};

template <typename T> class ValueTo : public Tween
{
	T _startValue;
	T _endValue;
	std::function<void(T)> _setter;

	ValueTo(T startValue, T endValue, float time, const std::function<void(T)>& setter);

	void update(float deltaTime) override;
	void finish() override;

public:
	Tween* from(T value);

	friend class Tweener;
};

template <typename T> Tween* Tweener::valueTo(T startValue, T endValue, float time, const std::function<void(T)>& setter)
{
	auto valueTo = new ValueTo<T>(startValue, endValue, time, setter);
	_tweens.push_back(valueTo);
	return valueTo;
}
template <typename T> ValueTo<T>::ValueTo(T startValue, T endValue, float time, const std::function<void(T)>& setter): Tween(time), _startValue(startValue),
	_endValue(endValue), _setter(setter) {}

template <typename T> void ValueTo<T>::update(float deltaTime)
{
	_setter(_startValue + (_endValue - _startValue) * Math::evaluateEase(_ease, _elapsed / _time));
	Tween::update(deltaTime);
}
template <typename T> void ValueTo<T>::finish()
{
	_setter(_endValue);
	Tween::finish();
}

template <typename T> Tween* ValueTo<T>::from(T value)
{
	_startValue = value;
	return this;
}
