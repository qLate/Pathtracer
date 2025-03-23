#include "Light.h"

#include "BufferController.h"
#include "Graphical.h"
#include "Material.h"
#include "Scene.h"

Light::Light(const glm::vec3& pos, const Color& color, float intensity) : Object(pos), _color(color), _intensity(intensity)
{
	Scene::lights.emplace_back(this);

	BufferController::markBufferForUpdate(BufferType::Lights);
}

void Light::setColor(const Color& color)
{
	_color = color;

	BufferController::markBufferForUpdate(BufferType::Lights);
}
void Light::setIntensity(float intensity)
{
	_intensity = intensity;

	BufferController::markBufferForUpdate(BufferType::Lights);
}

PointLight::PointLight(glm::vec3 pos, Color color, float intensity, float dis) : Light(pos, color, intensity), _dis(dis) { }

void PointLight::setDis(float dis)
{
	_dis = dis;

	BufferController::markBufferForUpdate(BufferType::Lights);
}

DirectionalLight::DirectionalLight(glm::vec3 rot, Color color, float intensity) : Light(glm::vec3(), color, intensity)
{
	Object::setRot(glm::quat(rot));
}
