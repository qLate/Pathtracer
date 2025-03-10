#pragma once

#include "SDL_events.h"

class Input
{
	static constexpr float MOVE_SPEED = 15.0f;
	static constexpr float MOVE_SPEED_BOOST = 3.0f;
	static constexpr float MOVE_ACCELERATION = 0.45f;

	static constexpr float KEY_ROTATION_SPEED = 90.0f;
	static constexpr float MOUSE_ROTATION_SPEED = 0.3f;

	inline static float currentMoveAcceleration = 1;

	inline static Uint8 lastKeyboardState[SDL_NUM_SCANCODES];
	inline static Uint8 keyboardState[SDL_NUM_SCANCODES];

public:
	static void updateInput();
	static void handleSDLEvent(const SDL_Event& event);

	static bool isKeyPressed(uint8_t key);
	static bool wasKeyPressed(uint8_t key);
	static bool wasKeyReleased(uint8_t key);
};
