#include "Camera.h"

#include <glm/gtx/quaternion.hpp>

#include "Pathtracer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, glm::vec2 size, float focalDistance, float lensRadius) : Object(pos), size(size), focalDistance(focalDistance), lensRadius(lensRadius),
                                                                                       bgColor(Color::black())
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Pathtracer::shaderP->setFloat3("cameraPos", pos);
	Pathtracer::shaderP->setFloat2("screenSize", size);
	Pathtracer::shaderP->setFloat("focalDistance", focalDistance);
	Pathtracer::shaderP->setFloat("lensRadius", lensRadius);

	onCameraRotate += [this] { Pathtracer::shaderP->setMatrix4X4("cameraRotMat", mat4_cast(this->rot)); };
	onCameraMove += [this] { Pathtracer::shaderP->setFloat3("cameraPos", this->pos); };
}

void Camera::setBackgroundColor(Color color)
{
	bgColor = color;
	Pathtracer::shaderP->setFloat4("bgColor", bgColor);
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
	Pathtracer::shaderP->setFloat2("screenSize", size);
}

glm::vec3 Camera::getScreenCenter() const
{
	return pos + forward() * focalDistance;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * size.y * up() - 0.5f * size.x * right();
}
