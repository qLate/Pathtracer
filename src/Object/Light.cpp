#include "Light.h"

#include "BufferController.h"
#include "Graphical.h"
#include "Material.h"
#include "MyMath.h"
#include "Scene.h"

Light::Light(const glm::vec3& pos, const Color& color, float intensity) : Object(pos), _color(color), _intensity(intensity)
{
	init();
}
Light::Light(const Light& orig) : Object(orig), _color(orig._color), _intensity(orig._intensity)
{
	init();
}
void Light::init()
{
	Scene::lights.emplace_back(this);

	BufferController::markBufferForUpdate(BufferType::Lights);
}

Light::~Light()
{
	if (!initialized()) return;

	std::erase(Scene::lights, this);

	BufferController::markBufferForUpdate(BufferType::Lights);
}

void Light::setPos(glm::vec3 pos, bool notify)
{
	Object::setPos(pos, notify);

	BufferController::markBufferForUpdate(BufferType::Lights);
}
void Light::setRot(glm::quat rot, bool notify)
{
	Object::setRot(rot, notify);

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

PointLight::PointLight(glm::vec3 pos, Color color, float intensity, float dis) : Light(pos, color, intensity), _dis(dis) {}
PointLight::PointLight(const PointLight& orig) : PointLight(orig.pos(), orig.color(), orig.intensity(), orig.dis()) {}

void PointLight::setDis(float dis)
{
	_dis = dis;

	BufferController::markBufferForUpdate(BufferType::Lights);
}

DirectionalLight::DirectionalLight(glm::vec3 dir, Color color, float intensity) : Light(glm::vec3(), color, intensity)
{
	init(quatLookAt(-dir, vec3::FORWARD));
}
DirectionalLight::DirectionalLight(const DirectionalLight& orig) : Light(orig.pos(), orig.color(), orig.intensity())
{
	init(orig.rot());
}
void DirectionalLight::init(glm::quat rot)
{
	Light::setRot(rot);
}
