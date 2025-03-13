#include "Tweener.h"

#include "MyTime.h"

void Tweener::update()
{
	_tweens.apply_changes();
	for (int i = 0; i < _tweens.size(); i++)
		_tweens[i]->update(Time::deltaTime);
}

Tween* Tweener::delayedCall(const std::function<void()>& function, float delay)
{
	auto delayedCall = new DelayedCall(function, delay);
	_tweens.push_back(delayedCall);
	return delayedCall;
}
Tween* Tweener::floatTo(float startValue, float endValue, float time, const std::function<void(float)>& setter)
{
	return valueTo(startValue, endValue, time, setter);
}
Tween* Tweener::vec2To(const glm::vec2& startValue, const glm::vec2& endValue, float time, const std::function<void(glm::vec2)>& setter)
{
	return valueTo(startValue, endValue, time, setter);
}
Tween* Tweener::colorTo(const Color& startValue, const Color& endValue, float time, const std::function<void(Color)>& setter)
{
	return valueTo(startValue, endValue, time, setter);
}

Tween::Tween(float time): _time(time) {}

void Tween::update(float deltaTime)
{
	// Delay
	if (_delay > deltaTime)
	{
		_delay -= deltaTime;
		return;
	}
	_delay = 0;

	// Update
	_elapsed += deltaTime;
	if (_elapsed >= _time)
	{
		_elapsed = _time;
		finish();
	}
}

void Tween::finish()
{
	if (_onComplete)
		_onComplete();

	Tweener::_tweens.erase_delayed(this);
	delete this;
}
void Tween::kill()
{
	Tweener::_tweens.erase_delayed(this);
	delete this;
}
Tween* Tween::setEase(EaseType ease)
{
	_ease = ease;
	return this;
}
Tween* Tween::setDelay(float delay)
{
	_delay = delay;
	return this;
}
Tween* Tween::onComplete(const std::function<void()>& onComplete)
{
	_onComplete = onComplete;
	return this;
}
Tween* Tween::setTarget(Object* target)
{
	_target = target;
	return this;
}

DelayedCall::DelayedCall(const std::function<void()>& function, float delay): Tween(delay), _function(function) {}
void DelayedCall::finish()
{
	_function();
	Tween::finish();
}
