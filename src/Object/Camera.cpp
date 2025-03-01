#include "Camera.h"

#include <glm/gtx/quaternion.hpp>

#include "MathExtensions.h"
#include "Raycast.h"
#include "Raytracer.h"
#include "Triangle.h"

Camera::Camera(glm::vec3 pos, float focalDistance, float lensRadius, glm::vec2 size) : Object(pos), focalDistance(focalDistance), lensRadius(lensRadius), size {size},
                                                                                       bgColor(Color::black())
{
	if (instance != nullptr)
		throw std::runtime_error("Camera object already exists.");
	instance = this;

	Raytracer::shader->setFloat3("cameraPos", pos);

	Raytracer::shader->setFloat("focalDistance", focalDistance);
	Raytracer::shader->setFloat("lensRadius", lensRadius);
	Raytracer::shader->setFloat2("screenSize", size);

	onCameraRotate += [this] { Raytracer::shader->setMatrix4X4("cameraRotMat", mat4_cast(this->rot)); };
	onCameraMove += [this] { Raytracer::shader->setFloat3("cameraPos", this->pos); };
	//onCameraMove += [this] { std::cout << to_string(this->pos); };
	//onCameraRotate += [this] { std::cout << to_string(this->rot); };
}

void Camera::setBackgroundColor(Color color)
{
	bgColor = color;
	Raytracer::shader->setFloat4("bgColor", bgColor);
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

glm::vec3 Camera::getScreenCenter() const
{
	return pos + forward() * focalDistance;
}
glm::vec3 Camera::getLeftBotCorner() const
{
	return getScreenCenter() - 0.5f * size.y * up() - 0.5f * size.x * right();
}
