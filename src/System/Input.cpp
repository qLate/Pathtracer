#include "Input.h"

#include <iostream>

#include "BufferController.h"
#include "Camera.h"
#include "Light.h"
#include "SDLHandler.h"
#include "glm/gtx/string_cast.hpp"
#include "MathExtensions.h"
#include "MyTime.h"
#include "Scene.h"

void Input::updateInput()
{
	if (!SDLHandler::windowFocused) return;

	auto& camera = Camera::instance;
	auto moveSpeed = MOVE_SPEED;
	auto moveDir = vec3::ZERO;
	auto keyboardState = SDL_GetKeyboardState(nullptr);

	// Movement
	if (keyboardState[SDL_SCANCODE_LSHIFT])
	{
		currentMoveAcceleration *= 1 + MOVE_ACCELERATION * Time::deltaTime;
		moveSpeed *= MOVE_SPEED_BOOST * currentMoveAcceleration;
	}
	else if (keyboardState[SDL_SCANCODE_LCTRL])
		moveSpeed /= MOVE_SPEED_BOOST;


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
		camera->translate(moveDir * moveSpeed * Time::deltaTime);
	else
		currentMoveAcceleration = 1;

	// Rotation
	if (keyboardState[SDL_SCANCODE_UP])
	{
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;
		auto newRot = glm::vec3(glm::clamp(rot.x + KEY_ROTATION_SPEED * Time::deltaTime, -90.0f, 90.0f), rot.y, rot.z);
		camera->setRot({newRot * DEG_TO_RAD});
	}
	if (keyboardState[SDL_SCANCODE_DOWN])
	{
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;
		auto newRot = glm::vec3(glm::clamp(rot.x - KEY_ROTATION_SPEED * Time::deltaTime, -90.0f, 90.0f), rot.y, rot.z);
		camera->setRot({newRot * DEG_TO_RAD});
	}
	if (keyboardState[SDL_SCANCODE_LEFT])
		camera->rotate({0, 0, KEY_ROTATION_SPEED * Time::deltaTime});
	if (keyboardState[SDL_SCANCODE_RIGHT])
		camera->rotate({0, 0, -KEY_ROTATION_SPEED * Time::deltaTime});

	// Other
	if (keyboardState[SDL_SCANCODE_Y])
	{
		std::cout << "Player is at:" << " pos " << vec3::to_string(camera->getPos()) << " rot " << vec3::to_string(camera->getRotVec4()) << std::endl;
	}

	if (keyboardState[SDL_SCANCODE_L] && !Scene::lights.empty())
	{
		Scene::lights[0]->setPos(camera->getPos());
		BufferController::updateLightsBuffer();

		std::cout << "Light set to position: " << vec3::to_string(Scene::lights[0]->getPos()) << std::endl;
	}
}

void Input::handleSDLEvent(const SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_F11)
		{
			SDLHandler::isFullscreen = !SDLHandler::isFullscreen;
			SDL_SetWindowFullscreen(SDLHandler::window, SDLHandler::isFullscreen ? 1 : 0);
		}
		else if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			SDLHandler::windowFocused = !SDLHandler::windowFocused;
			if (SDLHandler::windowFocused)
				SDL_SetRelativeMouseMode(SDL_TRUE);
			else
				SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}

	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT)
			currentMoveAcceleration = 1;
	}

	if (event.type == SDL_MOUSEMOTION && SDLHandler::windowFocused)
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
}
