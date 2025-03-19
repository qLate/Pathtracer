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

PointLight::PointLight(glm::vec3 pos, Color color, float intensity, float dis) : Light(pos, color, intensity), _dis(dis) { }
