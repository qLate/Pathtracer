#include "Camera.h"

#include "WindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "MyMath.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, float fov, float lensRadius) : Object(pos), _fov(fov), _lensRadius(lensRadius)
{
	init();
}
Camera::Camera(const Camera& orig) : Object(orig), _fov(orig._fov), _lensRadius(orig._lensRadius), _bgColor(orig._bgColor)
{
	init();
}
void Camera::init()
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	auto renderSize = WindowDrawer::currRenderSize();
	auto ratio = renderSize.x / (float)renderSize.y;

	setRatio(ratio);
	setFov(_fov);
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

	_pitch = glm::pitch(rot);
	_yaw = glm::yaw(rot);

	Renderer::resetSamples();
}
void Camera::setRot(float pitch, float yaw)
{
	setRot(angleAxis(glm::radians(yaw), vec3::UP) * angleAxis(glm::radians(pitch), vec3::RIGHT), false);

	_pitch = pitch;
	_yaw = yaw;

	Renderer::resetSamples();
}

void Camera::setRatio(float ratio)
{
	this->_ratio = ratio;

	Renderer::renderProgram()->setFloat2("viewSize", {ratio, 1});
	Renderer::resetSamples();
}
void Camera::setFov(float fov)
{
	_fov = fov;
	Renderer::renderProgram()->setFloat("focalDistance", getFocalDis());
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
	return _pos + forward() * getFocalDis();
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * _ratio * right() - 0.5f * up();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * _ratio * right() + screenPos.y * up();
	return normalize(screenPosWorld - _pos);
}
glm::vec3 Camera::getMouseDir() const
{
	auto normalizedMousePos = Input::getSceneMousePos() / (glm::vec2)WindowDrawer::currRenderSize();
	normalizedMousePos.y = 1 - normalizedMousePos.y;
	return getDir(normalizedMousePos);
}

float Camera::getFocalDis() const
{
	return 0.5f / tan(glm::radians(_fov) * 0.5f);
}
glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAtLH(_pos, _pos + forward(), up());
}
glm::mat4 Camera::getProjectionMatrix() const
{
	float nearPlane = 0.1f;
	return glm::infinitePerspectiveLH(glm::radians(_fov), _ratio, nearPlane);
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
