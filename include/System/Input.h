#pragma once

#include <SDL2/include/SDL_events.h>

class Input
{
	static constexpr float MOVE_SPEED = 15.0f;
	static constexpr float MOVE_SPEED_BOOST = 3.0f;
	static constexpr float MOVE_ACCELERATION = 0.45f;

	static constexpr float KEY_ROTATION_SPEED = 90.0f;
	static constexpr float MOUSE_ROTATION_SPEED = 0.4f;

	inline static float currentMoveAcceleration = 1;

public:
	static void updateInput();
	static void handleSDLEvent(const SDL_Event& event);
};
