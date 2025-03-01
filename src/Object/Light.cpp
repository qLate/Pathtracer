#include "Light.h"

#include "Graphical.h"
#include "Material.h"
#include "Scene.h"

Light::Light(const glm::vec3& pos, const Color& color, float intensity) : Object(pos), color(color), intensity(intensity)
{
	Scene::lights.emplace_back(this);
}

PointLight::PointLight(glm::vec3 pos, Color color, float intensity, float distance) : Light(pos, color, intensity), distance(distance) { }

AreaLight::AreaLight(glm::vec3 pos, Color color, float intensity, float distance, glm::vec3 size, glm::vec3 pointSize) : Light(pos, color, intensity), distance(distance),
	size(size), pointSize(pointSize) {}
