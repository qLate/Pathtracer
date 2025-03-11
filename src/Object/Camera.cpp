#include "Camera.h"

#include <glm/gtx/quaternion.hpp>

#include "Renderer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, glm::vec2 size, float focalDistance, float lensRadius) : Object(pos), size(size), focalDistance(focalDistance), lensRadius(lensRadius),
                                                                                       bgColor(Color::black())
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Renderer::shaderP->setFloat3("cameraPos", pos);
	Renderer::shaderP->setFloat2("screenSize", size);
	Renderer::shaderP->setFloat("focalDistance", focalDistance);
	Renderer::shaderP->setFloat("lensRadius", lensRadius);

	onCameraRotate += [this] { Renderer::shaderP->setMatrix4X4("cameraRotMat", mat4_cast(this->rot)); };
	onCameraMove += [this] { Renderer::shaderP->setFloat3("cameraPos", this->pos); };
}

void Camera::setBackgroundColor(Color color)
{
	bgColor = color;
	Renderer::shaderP->setFloat4("bgColor", bgColor);
}

void Camera::setRot(glm::quat rot)
{
	Object::setRot(rot);
	onCameraRotate();
}
void Camera::setPos(glm::vec3 pos)
{
	Object::setPos(pos);
	onCameraMove();
}
void Camera::setSize(glm::vec2 size)
{
	this->size = size;
	Renderer::shaderP->setFloat2("screenSize", size);
}

glm::vec3 Camera::getScreenCenter() const
{
	return pos + forward() * focalDistance;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * size.y * up() - 0.5f * size.x * right();
}
