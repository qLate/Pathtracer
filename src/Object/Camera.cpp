#include "Camera.h"

#include "WindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "SDLHandler.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, float size, float focalDis, float lensRadius) : Object(pos), _size(size), _focalDis(focalDis), _lensRadius(lensRadius)
{
	init();
}
Camera::Camera(const Camera& orig) : Object(orig), _size(orig._size), _focalDis(orig._focalDis), _lensRadius(orig._lensRadius), _bgColor(orig._bgColor)
{
	init();
}
void Camera::init()
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	auto renderSize = WindowDrawer::currRenderSize();
	_ratio = glm::vec2(renderSize.x / (float)renderSize.y, 1);

	setSize(_size);
	setFocalDistance(_focalDis);
	setLensRadius(_lensRadius);
	setBgColor(_bgColor);
}

Camera::~Camera()
{
	if (!initialized()) return;
	instance = nullptr;
}

void Camera::setPos(glm::vec3 pos, bool notify)
{
	Object::setPos(pos, false);

	Renderer::resetSamples();
}
void Camera::setRot(glm::quat rot, bool notify)
{
	Object::setRot(rot, false);

	Renderer::resetSamples();
}

void Camera::setRatio(glm::vec2 ratio)
{
	this->_ratio = ratio;

	Renderer::renderProgram()->setFloat2("viewSize", _size * ratio);
	Renderer::resetSamples();
}
void Camera::setSize(float size)
{
	this->_size = size;
	Renderer::renderProgram()->setFloat2("viewSize", size * _ratio);
	Renderer::resetSamples();
}
void Camera::setFocalDistance(float focalDistance)
{
	_focalDis = focalDistance;
	Renderer::renderProgram()->setFloat("focalDistance", _focalDis);
	Renderer::resetSamples();
}
void Camera::setLensRadius(float lensRadius)
{
	_lensRadius = lensRadius;
	Renderer::renderProgram()->setFloat("lensRadius", _lensRadius);
	Renderer::resetSamples();
}
void Camera::setBgColor(Color color)
{
	_bgColor = color;
	Renderer::renderProgram()->setFloat3("bgColor", _bgColor);
	Renderer::resetSamples();
}

glm::vec3 Camera::getScreenCenter() const
{
	return _pos + forward() * _focalDis;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * _size * _ratio.y * up() - 0.5f * _size * _ratio.x * right();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * _size * _ratio.x * right() + screenPos.y * _size * _ratio.y * up();
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
	float fov = 2.0f * glm::degrees(glm::atan(_size * _ratio.y * 0.5f / _focalDis));
	float nearPlane = 0.1f;
	float farPlane = 10000.0f;

	return glm::perspective(glm::radians(fov), _ratio.x, nearPlane, farPlane);
}

glm::vec2 Camera::worldToViewportPos(const glm::vec3& worldPos) const
{
	auto viewMatrix = getViewMatrix();
	auto projectionMatrix = getProjectionMatrix();

	auto screenPos = projectionMatrix * viewMatrix * glm::vec4(worldPos, 1);
	screenPos /= screenPos.w;
	if (screenPos.z < 0 || screenPos.z > 1) return glm::vec2(-1);

	screenPos = 0.5f * screenPos + 0.5f;
	screenPos.y = 1 - screenPos.y;
	return screenPos;
}
glm::ivec2 Camera::worldToScreenPos(const glm::vec3& worldPos) const
{
	auto screenPos = worldToViewportPos(worldPos);
	return screenPos * (glm::vec2)WindowDrawer::currRenderSize();
}
