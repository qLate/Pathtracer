#include "Camera.h"

#include <glm/gtx/quaternion.hpp>

#include "ImGUIWindowDrawer.h"
#include "Input.h"
#include "Renderer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, glm::vec2 size, float focalDistance, float lensRadius) : Object(pos), size(size), focalDistance(focalDistance), lensRadius(lensRadius)
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Renderer::renderProgram->setFloat2("screenSize", size);
	Renderer::renderProgram->setFloat("focalDistance", focalDistance);
	Renderer::renderProgram->setFloat("lensRadius", lensRadius);
}

void Camera::setBackgroundColor(Color color)
{
	bgColor = color;
	Renderer::renderProgram->setFloat4("bgColor", bgColor);
}

void Camera::setSize(glm::vec2 size)
{
	this->size = size;
	Renderer::renderProgram->setFloat2("screenSize", size);
}

glm::vec3 Camera::getScreenCenter() const
{
	return pos + forward() * focalDistance;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * size.y * up() - 0.5f * size.x * right();
}
glm::vec3 Camera::getDir(const glm::vec2& screenPos) const
{
	auto lbWorld = getLeftBotCorner();
	auto screenPosWorld = lbWorld + screenPos.x * size.x * right() + screenPos.y * size.y * up();
	return normalize(screenPosWorld - pos);
}
glm::vec3 Camera::getMouseDir() const
{
	auto normalizedMousePos = Input::getSceneMousePos() / (glm::vec2)ImGUIWindowDrawer::currRenderSize;
	normalizedMousePos.y = 1 - normalizedMousePos.y;
	return getDir(normalizedMousePos);
}
