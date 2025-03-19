#include "Camera.h"

#include "ImGUIWindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, glm::vec2 size, float focalDistance, float lensRadius) : Object(pos), _size(size), _focalDis(focalDistance), _lensRadius(lensRadius)
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Renderer::renderProgram()->setFloat2("screenSize", size);
	Renderer::renderProgram()->setFloat("focalDistance", focalDistance);
	Renderer::renderProgram()->setFloat("lensRadius", lensRadius);
}

void Camera::setPos(glm::vec3 pos, bool notify)
{
	Object::setPos(pos, notify);
}
void Camera::setRot(glm::quat rot, bool notify)
{
	Object::setRot(rot, notify);
}
void Camera::setScale(glm::vec3 scale, bool notify)
{
	Object::setScale(scale, notify);
}

void Camera::setSize(glm::vec2 size)
{
	this->_size = size;
	Renderer::renderProgram()->setFloat2("screenSize", size);
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
	return getScreenCenter() - 0.5f * _size.y * up() - 0.5f * _size.x * right();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * _size.x * right() + screenPos.y * _size.y * up();
	return normalize(screenPosWorld - _pos);
}
glm::vec3 Camera::getMouseDir() const
{
	auto normalizedMousePos = Input::getSceneMousePos() / (glm::vec2)ImGUIWindowDrawer::currRenderSize();
	normalizedMousePos.y = 1 - normalizedMousePos.y;
	return getDir(normalizedMousePos);
}
