#include "Camera.h"

#include "WindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, glm::vec2 ratio, float focalDistance, float lensRadius) : Object(pos), _ratio(ratio), _focalDis(focalDistance), _lensRadius(lensRadius)
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Renderer::renderProgram()->setFloat2("screenSize", ratio);
	Renderer::renderProgram()->setFloat("focalDistance", focalDistance);
	Renderer::renderProgram()->setFloat("lensRadius", lensRadius);
}

void Camera::setPos(glm::vec3 pos, bool notify)
{
	Object::setPos(pos, false);
}
void Camera::setRot(glm::quat rot, bool notify)
{
	Object::setRot(rot, false);
}
void Camera::setScale(glm::vec3 scale, bool notify)
{
	Object::setScale(scale, false);
}

void Camera::setRatio(glm::vec2 ratio)
{
	this->_ratio = ratio;
	Renderer::renderProgram()->setFloat2("screenSize", ratio);
}
void Camera::setFocalDistance(float focalDistance) { _focalDis = focalDistance; }
void Camera::setLensRadius(float lensRadius) { _lensRadius = lensRadius; }
void Camera::setBgColor(Color color)
{
	_bgColor = color;
	Renderer::renderProgram()->setFloat4("bgColor", _bgColor);
}

glm::vec3 Camera::getScreenCenter() const
{
	return _pos + forward() * _focalDis;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * _ratio.y * up() - 0.5f * _ratio.x * right();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * _ratio.x * right() + screenPos.y * _ratio.y * up();
	return normalize(screenPosWorld - _pos);
}
glm::vec3 Camera::getMouseDir() const
{
	auto normalizedMousePos = Input::getSceneMousePos() / (glm::vec2)WindowDrawer::currRenderSize();
	normalizedMousePos.y = 1 - normalizedMousePos.y;
	return getDir(normalizedMousePos);
}

glm::mat4 Camera::getViewMatrix() const
{
	return lookAt(_pos, _pos + forward(), up());
}
glm::mat4 Camera::getProjectionMatrix() const
{
	float fov = 2.0f * glm::degrees(glm::atan(_ratio.y * 0.5f / _focalDis));
	float nearPlane = 0.1f;
	float farPlane = 10000.0f;

	return glm::perspective(glm::radians(fov), _ratio.x, nearPlane, farPlane);
}
