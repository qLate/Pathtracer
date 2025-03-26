#include "Camera.h"

#include "WindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, float focalDistance, float lensRadius) : Object(pos), _focalDis(focalDistance), _lensRadius(lensRadius)
{
	init();
}
Camera::Camera(const Camera& orig) : Object(orig), _viewSize(orig._viewSize), _focalDis(orig._focalDis), _lensRadius(orig._lensRadius), _bgColor(orig._bgColor)
{
	init();
}
void Camera::init()
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	auto renderSize = WindowDrawer::currRenderSize();
	_viewSize = {renderSize.x / (float)renderSize.y, 1};

	setViewSize(_viewSize);
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
}
void Camera::setRot(glm::quat rot, bool notify)
{
	Object::setRot(rot, false);
}
void Camera::setScale(glm::vec3 scale, bool notify)
{
	Object::setScale(scale, false);
}

void Camera::setViewSize(glm::vec2 viewSize)
{
	this->_viewSize = viewSize;
	Renderer::renderProgram()->setFloat2("viewSize", viewSize);
}
void Camera::setFocalDistance(float focalDistance)
{
	_focalDis = focalDistance;
	Renderer::renderProgram()->setFloat("focalDistance", _focalDis);
}
void Camera::setLensRadius(float lensRadius)
{
	_lensRadius = lensRadius;
	Renderer::renderProgram()->setFloat("lensRadius", _lensRadius);
}
void Camera::setBgColor(Color color)
{
	_bgColor = color;
	Renderer::renderProgram()->setFloat3("bgColor", _bgColor);
}

glm::vec3 Camera::getScreenCenter() const
{
	return _pos + forward() * _focalDis;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * _viewSize.y * up() - 0.5f * _viewSize.x * right();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * _viewSize.x * right() + screenPos.y * _viewSize.y * up();
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
	float fov = 2.0f * glm::degrees(glm::atan(_viewSize.y * 0.5f / _focalDis));
	float nearPlane = 0.1f;
	float farPlane = 10000.0f;

	return glm::perspective(glm::radians(fov), _viewSize.x, nearPlane, farPlane);
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
