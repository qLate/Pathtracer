#include "Input.h"

#include <iostream>

#include "BufferController.h"
#include "Camera.h"
#include "Light.h"
#include "SDLHandler.h"
#include "glm/gtx/string_cast.hpp"
#include "MyMath.h"
#include "MyTime.h"
#include "Scene.h"

void Input::update()
{
	updateInputState();
	updateMovement();
}
void Input::updateInputState()
{
	memcpy(lastKeyboardState, keyboardState, SDL_NUM_SCANCODES);
	mouseWheelChange = 0;

	auto keyboardStatePtr = SDL_GetKeyboardState(nullptr);
	memcpy(keyboardState, keyboardStatePtr, SDL_NUM_SCANCODES);
}
void Input::updateMovement()
{
	if (!SDLHandler::sceneFocused) return;

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
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			SDLHandler::sceneFocused = !SDLHandler::sceneFocused;
			if (SDLHandler::sceneFocused)
				SDL_SetRelativeMouseMode(SDL_TRUE);
			else
				SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		else if (event.key.keysym.sym == SDLK_f)
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
	}

	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT)
			currMoveAcceleration = 1;
	}

	// Mouse rotation
	if (event.type == SDL_MOUSEMOTION && SDLHandler::sceneFocused)
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
	return !lastKeyboardState[key] && keyboardState[key];
}
bool Input::wasKeyReleased(uint8_t key)
{
	return lastKeyboardState[key] && !keyboardState[key];
}
