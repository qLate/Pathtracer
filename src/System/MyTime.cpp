#include "MyTime.h"

#include "SDL_timer.h"
#include "glm/common.hpp"

void Time::update()
{
	auto new_time = (float)SDL_GetTicks() / 1000.0f;
	deltaTime = glm::clamp(new_time - time, 0.0f, MAX_DELTA_TIME);
	time = new_time;
}
