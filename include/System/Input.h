#pragma once

#include "SDL_events.h"
#include "glm/vec2.hpp"

class Input
{
	static constexpr float MOVE_SPEED = 15.0f;
	static constexpr float MOVE_SPEED_BOOST = 3.0f;
	static constexpr float MOVE_ACCELERATION = 0.45f;

	static constexpr float MOUSE_ROTATION_SPEED = 0.3f;

	inline static float _moveSpeedMult = 1;
	inline static float _currMoveAcceleration = 1;

	inline static Uint8 _lastKeyboardState[SDL_NUM_SCANCODES];
	inline static Uint8 _keyboardState[SDL_NUM_SCANCODES];

	inline static bool _lastMouseLeftState = false;
	inline static bool _lastMouseRightState = false;
	inline static bool _mouseLeftState = false;
	inline static bool _mouseRightState = false;

	inline static Sint8 _mouseWheelChange = 0;

	static void update();
	static void updateInputState();
	static void updateMovement();

	static void handleSDLEvent(const SDL_Event& event);

public:
	static bool isKeyPressed(uint8_t key);
	static bool wasKeyPressed(uint8_t key);
	static bool wasKeyReleased(uint8_t key);

	static bool isMouseDown(bool left);
	static bool wasMousePressed(bool left);
	static bool wasMouseReleased(bool left);

	static glm::vec2 getSceneMousePos();
	static float getMouseWheelChange();

	friend class ImGUIWindowDrawer;
	friend class Program;
	friend class SDLHandler;
};
