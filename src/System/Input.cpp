#include "Input.h"

#include <iostream>

#include "BufferController.h"
#include "Camera.h"
#include "Debug.h"
#include "Light.h"
#include "SDLHandler.h"
#include "glm/gtx/string_cast.hpp"
#include "MyMath.h"
#include "MyTime.h"
#include "Physics.h"
#include "Scene.h"
#include "Triangle.h"

void Input::update()
{
	updateInputState();
	updateMovement();
}
void Input::updateInputState()
{
	memcpy(_lastKeyboardState, keyboardState, SDL_NUM_SCANCODES);
	auto keyboardStatePtr = SDL_GetKeyboardState(nullptr);
	memcpy(keyboardState, keyboardStatePtr, SDL_NUM_SCANCODES);

	_lastMouseLeftState = mouseLeftState;
	_lastMouseRightState = mouseRightState;

	mouseWheelChange = 0;
}
void Input::updateMovement()
{
	if (!ImGUIHandler::isWindowFocused(WindowType::Scene)) return;

	auto camera = Camera::instance;
	auto finalMoveSpeed = MOVE_SPEED * moveSpeedMult;
	auto moveDir = vec3::ZERO;

	if (keyboardState[SDL_SCANCODE_LSHIFT])
	{
		currMoveAcceleration *= 1 + MOVE_ACCELERATION * Time::deltaTime;
		finalMoveSpeed *= MOVE_SPEED_BOOST * currMoveAcceleration;
	}
	else if (keyboardState[SDL_SCANCODE_LCTRL])
		finalMoveSpeed /= MOVE_SPEED_BOOST;

	if (keyboardState[SDL_SCANCODE_W])
		moveDir += camera->forward();
	if (keyboardState[SDL_SCANCODE_S])
		moveDir += camera->backward();
	if (keyboardState[SDL_SCANCODE_A])
		moveDir += camera->left();
	if (keyboardState[SDL_SCANCODE_D])
		moveDir += camera->right();
	if (keyboardState[SDL_SCANCODE_SPACE])
		moveDir += camera->up();
	if (keyboardState[SDL_SCANCODE_R])
		moveDir += camera->down();

	if (moveDir != vec3::ZERO)
		camera->translate(moveDir * finalMoveSpeed * Time::deltaTime);
	else
		currMoveAcceleration = 1;
}

void Input::handleSDLEvent(const SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_f)
		{
			SDLHandler::isFullscreen = !SDLHandler::isFullscreen;
			if (SDLHandler::isFullscreen)
				SDL_SetWindowFullscreen(SDLHandler::window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(SDLHandler::window, 0);
		}
		else if (event.key.keysym.sym == SDLK_l && !Scene::lights.empty())
		{
			Scene::lights[0]->setPos(Camera::instance->getPos());
			BufferController::updateLightsBuffer();
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			std::cout << "Player is at:" << " pos " << to_string(Camera::instance->getPos()) << " rot " << to_string(Camera::instance->getRot()) << '\n';
		}
		else if (event.key.keysym.sym == SDLK_u)
		{
			auto hit = Physics::raycast(Camera::instance->getPos(), Camera::instance->forward());
		}
	}

	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT)
			currMoveAcceleration = 1;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			mouseLeftState = true;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			mouseRightState = true;

			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDLHandler::mouseAttachedToScene = true;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			mouseLeftState = false;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			mouseRightState = false;

			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDLHandler::mouseAttachedToScene = false;
		}
	}
	if (event.type == SDL_MOUSEMOTION && ImGUIHandler::isWindowFocused(WindowType::Scene) && isMouseDown(false))
	{
		auto dx = (float)event.motion.xrel;
		auto dy = (float)event.motion.yrel;

		auto& camera = Camera::instance;
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;

		auto newX = glm::clamp(rot.x - dy * MOUSE_ROTATION_SPEED, -90.0f, 90.0f);
		auto newY = rot.y;
		auto newZ = rot.z - dx * MOUSE_ROTATION_SPEED;

		auto newRot = glm::vec3(newX, newY, newZ);
		camera->setRot({newRot * DEG_TO_RAD});
	}

	if (event.type == SDL_MOUSEWHEEL)
	{
		mouseWheelChange += event.wheel.y;

		if (isKeyPressed(SDL_SCANCODE_LALT))
		{
			moveSpeedMult *= 1 + event.wheel.y * 0.3f;
			moveSpeedMult = glm::clamp(moveSpeedMult, 0.1f, 20.0f);
		}
	}
}

bool Input::isKeyPressed(uint8_t key)
{
	return keyboardState[key];
}
bool Input::wasKeyPressed(uint8_t key)
{
	return !_lastKeyboardState[key] && keyboardState[key];
}
bool Input::wasKeyReleased(uint8_t key)
{
	return _lastKeyboardState[key] && !keyboardState[key];
}

bool Input::isMouseDown(bool left)
{
	return left ? mouseLeftState : mouseRightState;
}
bool Input::wasMousePressed(bool left)
{
	return left ? !_lastMouseLeftState && mouseLeftState : !_lastMouseRightState && mouseRightState;
}
bool Input::wasMouseReleased(bool left)
{
	return left ? _lastMouseLeftState && !mouseLeftState : _lastMouseRightState && !mouseRightState;
}

float Input::getMouseWheelChange()
{
	return mouseWheelChange;
}
