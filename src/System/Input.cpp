#include "Input.h"

#include "Camera.h"
#include "SDLHandler.h"
#include "glm/gtx/string_cast.hpp"
#include "MathExtensions.h"
#include "MyTime.h"

float defaultMoveSpeed = 15.0f;
float keyRotationSpeed = 90.0f;
float mouseRotationSpeed = 0.4f;

void Input::updateInput()
{
	if (!SDLHandler::windowFocused) return;

	auto& camera = Camera::instance;
	auto moveSpeed = defaultMoveSpeed;
	auto keyboardState = SDL_GetKeyboardState(nullptr);

	// Movement
	if (keyboardState[SDL_SCANCODE_LSHIFT])
		moveSpeed *= 4;
	else if (keyboardState[SDL_SCANCODE_LCTRL])
		moveSpeed /= 4;

	if (keyboardState[SDL_SCANCODE_W])
		camera->translate(camera->forward() * moveSpeed * Time::clampedDeltaTime);
	if (keyboardState[SDL_SCANCODE_S])
		camera->translate(camera->backward() * moveSpeed * Time::clampedDeltaTime);
	if (keyboardState[SDL_SCANCODE_A])
		camera->translate(camera->left() * moveSpeed * Time::clampedDeltaTime);
	if (keyboardState[SDL_SCANCODE_D])
		camera->translate(camera->right() * moveSpeed * Time::clampedDeltaTime);
	if (keyboardState[SDL_SCANCODE_SPACE])
		camera->translate(camera->up() * moveSpeed * Time::clampedDeltaTime);
	if (keyboardState[SDL_SCANCODE_R])
		camera->translate(camera->down() * moveSpeed * Time::clampedDeltaTime);

	// Rotation
	if (keyboardState[SDL_SCANCODE_UP])
	{
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;
		auto newRot = glm::vec3(glm::clamp(rot.x + keyRotationSpeed * Time::deltaTime, -90.0f, 90.0f), rot.y, rot.z);
		camera->setRot({newRot * DEG_TO_RAD});
	}
	if (keyboardState[SDL_SCANCODE_DOWN])
	{
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;
		auto newRot = glm::vec3(glm::clamp(rot.x - keyRotationSpeed * Time::deltaTime, -90.0f, 90.0f), rot.y, rot.z);
		camera->setRot({newRot * DEG_TO_RAD});
	}
	if (keyboardState[SDL_SCANCODE_LEFT])
		camera->rotate({0, 0, keyRotationSpeed * Time::clampedDeltaTime});
	if (keyboardState[SDL_SCANCODE_RIGHT])
		camera->rotate({0, 0, -keyRotationSpeed * Time::clampedDeltaTime});

	if (keyboardState[SDL_SCANCODE_Y])
	{
		camera->getPos() = {0.5, 0, 0.5};
		camera->getRot() = {1, 0, 0, 0};
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
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			SDLHandler::windowFocused = !SDLHandler::windowFocused;
			if (SDLHandler::windowFocused)
				SDL_SetRelativeMouseMode(SDL_TRUE);
			else
				SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}

	if (event.type == SDL_MOUSEMOTION && SDLHandler::windowFocused)
	{
		auto dx = (float)event.motion.xrel;
		auto dy = (float)event.motion.yrel;

		auto& camera = Camera::instance;
		auto rot = eulerAngles(camera->getRot()) * RAD_TO_DEG;

		auto newX = glm::clamp(rot.x - dy * mouseRotationSpeed, -90.0f, 90.0f);
		auto newY = rot.y;
		auto newZ = rot.z - dx * mouseRotationSpeed;

		auto newRot = glm::vec3(newX, newY, newZ);
		camera->setRot({newRot * DEG_TO_RAD});
	}
}
