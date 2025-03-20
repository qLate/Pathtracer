#include "Input.h"

#include "BufferController.h"
#include "Camera.h"
#include "Graphical.h"
#include "ImGUIWindowDrawer.h"
#include "Light.h"
#include "SDLHandler.h"
#include "glm/gtx/string_cast.hpp"
#include "MyMath.h"
#include "MyTime.h"
#include "Physics.h"
#include "Scene.h"
#include "Triangle.h"
#include "Utils.h"

void Input::update()
{
	updateInputState();
	updateMovement();
}
void Input::updateInputState()
{
	memcpy(_lastKeyboardState, _keyboardState, SDL_NUM_SCANCODES);
	auto keyboardStatePtr = SDL_GetKeyboardState(nullptr);
	memcpy(_keyboardState, keyboardStatePtr, SDL_NUM_SCANCODES);

	_lastMouseLeftState = _mouseLeftState;
	_lastMouseRightState = _mouseRightState;

	_mouseWheelChange = 0;
}
void Input::updateMovement()
{
	if (!ImGUIHandler::isWindowFocused(WindowType::Scene)) return;

	auto camera = Camera::instance;
	auto finalMoveSpeed = MOVE_SPEED * _moveSpeedMult;
	auto moveDir = vec3::ZERO;

	if (_keyboardState[SDL_SCANCODE_LSHIFT])
	{
		_currMoveAcceleration *= 1 + MOVE_ACCELERATION * Time::deltaTime();
		finalMoveSpeed *= MOVE_SPEED_BOOST * _currMoveAcceleration;

		BufferController::markBufferForUpdate(BufferType::Objects);

		TimeMeasurerGL tm;
		BVH::rebuildBVH();
		tm.printElapsedFromLast("BVH rebuild time: ");
	}
	else if (_keyboardState[SDL_SCANCODE_LCTRL])
		finalMoveSpeed /= MOVE_SPEED_BOOST;

	if (_keyboardState[SDL_SCANCODE_W])
		moveDir += camera->forward();
	if (_keyboardState[SDL_SCANCODE_S])
		moveDir += camera->backward();
	if (_keyboardState[SDL_SCANCODE_A])
		moveDir += camera->left();
	if (_keyboardState[SDL_SCANCODE_D])
		moveDir += camera->right();
	if (_keyboardState[SDL_SCANCODE_SPACE])
		moveDir += camera->up();
	if (_keyboardState[SDL_SCANCODE_R])
		moveDir += camera->down();

	if (moveDir != vec3::ZERO)
		camera->translate(moveDir * finalMoveSpeed * Time::deltaTime());
	else
		_currMoveAcceleration = 1;
}

void Input::handleSDLEvent(const SDL_Event& event)
{
	auto camera = Camera::instance;
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_f)
		{
			SDLHandler::setFullscreen(!SDLHandler::isFullscreen());
		}
		else if (event.key.keysym.sym == SDLK_l && !Scene::lights.empty())
		{
			Scene::lights[0]->setPos(camera->pos());
			BufferController::updateLights();
		}
		else if (event.key.keysym.sym == SDLK_y)
		{
			Debug::log("Player is at: pos ", to_string(camera->pos()), " rot ", to_string(camera->rot()));
		}
	}

	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_LSHIFT)
			_currMoveAcceleration = 1;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			_mouseLeftState = true;

			if (ImGUIHandler::isWindowFocused(WindowType::Scene))
			{
				auto hit = Physics::raycast(camera->pos(), camera->getMouseDir());
				if (hit.hit)
					hit.object->translate(glm::vec3(0, 0, 1));
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			_mouseRightState = true;

			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDLHandler::setAttachMouseToScene(true);
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			_mouseLeftState = false;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			_mouseRightState = false;

			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDLHandler::setAttachMouseToScene(false);
		}
	}
	if (event.type == SDL_MOUSEMOTION && ImGUIHandler::isWindowFocused(WindowType::Scene) && isMouseDown(false))
	{
		auto dx = (float)event.motion.xrel;
		auto dy = (float)event.motion.yrel;

		auto rot = eulerAngles(camera->rot()) * RAD_TO_DEG;

		auto newX = glm::clamp(rot.x - dy * MOUSE_ROTATION_SPEED, -90.0f, 90.0f);
		auto newY = rot.y;
		auto newZ = rot.z - dx * MOUSE_ROTATION_SPEED;

		auto newRot = glm::vec3(newX, newY, newZ);
		camera->setRot({newRot * DEG_TO_RAD});
	}

	if (event.type == SDL_MOUSEWHEEL)
	{
		_mouseWheelChange += event.wheel.y;

		if (isKeyPressed(SDL_SCANCODE_LALT))
		{
			_moveSpeedMult *= 1 + event.wheel.y * 0.3f;
			_moveSpeedMult = glm::clamp(_moveSpeedMult, 0.1f, 20.0f);
		}
	}
}

bool Input::isKeyPressed(uint8_t key)
{
	return _keyboardState[key];
}
bool Input::wasKeyPressed(uint8_t key)
{
	return !_lastKeyboardState[key] && _keyboardState[key];
}
bool Input::wasKeyReleased(uint8_t key)
{
	return _lastKeyboardState[key] && !_keyboardState[key];
}

bool Input::isMouseDown(bool left)
{
	return left ? _mouseLeftState : _mouseRightState;
}
bool Input::wasMousePressed(bool left)
{
	return left ? !_lastMouseLeftState && _mouseLeftState : !_lastMouseRightState && _mouseRightState;
}
bool Input::wasMouseReleased(bool left)
{
	return left ? _lastMouseLeftState && !_mouseLeftState : _lastMouseRightState && !_mouseRightState;
}
glm::vec2 Input::getSceneMousePos()
{
	return ImGUIHandler::getRelativeMousePos(WindowType::Scene);
}

float Input::getMouseWheelChange()
{
	return _mouseWheelChange;
}
