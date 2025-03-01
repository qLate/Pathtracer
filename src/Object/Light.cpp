#include "Light.h"

#include "GraphicalObject.h"
#include "Material.h"
#include "Raycast.h"
#include "Scene.h"

Light::Light(const glm::vec3& pos, const Color& color, float intensity) : Object(pos), color(color), intensity(intensity), colorIntensified(color * intensity)
{
	Scene::lights.emplace_back(this);
}

PointLight::PointLight(glm::vec3 pos, Color color, float intensity, float distance) : Light(pos, color, intensity), distance(distance) { }

void PointLight::getIlluminationAtPoint(const Ray& ray, Color& inColor, Color& inSpecular)
{
	auto dir = pos - ray.interPoint;
	auto dist = length(dir);
	if (dist > distance)
		return;

	dir = normalize(dir);
	if (Raycast::castShadowRay({pos, -dir, dist}))
		return;

	auto distanceImpact = (float)std::min(1 - (dist / distance), 1.f);
	auto lightFacingAtPoint = std::max(dot(dir, ray.surfaceNormal), 0.f);
	inColor += (distanceImpact * lightFacingAtPoint) * colorIntensified;

	auto h = normalize(dir - ray.dir);
	inSpecular += distanceImpact * (float)std::pow(std::max(dot(h, ray.surfaceNormal), 0.0f), ray.closestMat->specularDegree) * colorIntensified;
}

AreaLight::AreaLight(glm::vec3 pos, Color color, float intensity, float distance, glm::vec3 size, glm::vec3 pointSize) : Light(pos, color, intensity),
distance(distance), size(size), pointSize(pointSize)
{
	float dx = size.x / pointSize.x;
	float dy = size.y / pointSize.y;
	float dz = size.z / pointSize.z;
	for (int x = 0; x < pointSize.x; ++x)
	{
		for (int y = 0; y < pointSize.y; ++y)
		{
			for (int z = 0; z < pointSize.z; ++z)
			{
				auto point = pos - size / 2.0f + glm::vec3(x * dx, y * dy, z * dz);
				points.emplace_back(point);
			}
		}
	}
	colorIntensified /= (float)points.size();
}

void AreaLight::getIlluminationAtPoint(const Ray& ray, Color& inColor, Color& inSpecular)
{
	for (const auto& lightPoint : points)
	{
		auto dist = length(lightPoint - ray.interPoint);
		if (dist > distance)
			continue;

		auto dir = normalize(lightPoint - ray.interPoint);
		if (Raycast::castShadowRay({lightPoint, -dir, dist}))
			continue;

		auto distanceImpact = (float)std::max(1 - (dist / distance), 0.f);
		auto lightFacingAtPoint = std::max(dot(dir, ray.surfaceNormal), 0.f);
		inColor += distanceImpact * lightFacingAtPoint * colorIntensified;

		auto H = normalize(dir - ray.dir);
		inSpecular += distanceImpact * (float)std::pow(std::max(dot(H, ray.surfaceNormal), 0.0f), ray.closestMat->specularDegree) * colorIntensified;
	}
}

GlobalLight::GlobalLight(glm::vec3 direction, Color color, float intensity) : Light({}, color, intensity), direction(direction) {}

void GlobalLight::getIlluminationAtPoint(const Ray& ray, Color& inColor, Color& inSpecular)
{
	if (Raycast::castShadowRay({ray.interPoint, direction, FLT_MAX}))
		return;

	auto light = std::max(dot(direction, ray.surfaceNormal), 0.f);
	inColor += light * colorIntensified;

	auto h = normalize(direction - ray.dir);
	inSpecular += (float)std::pow(std::max(dot(h, ray.surfaceNormal), 0.0f), ray.closestMat->specularDegree) * colorIntensified;
}
EverywhereLight::EverywhereLight(const Color& color, float intensity): Light({}, color, intensity) {}

void EverywhereLight::getIlluminationAtPoint(const Ray& ray, Color& inColor, Color& inSpecular)
{
	inColor += colorIntensified;
}

std::pair<Color, Color> getIlluminationAtPoint(const Ray& ray)
{
	Color diffuse{};
	Color specular{};
	for (const auto& light : Scene::lights)
	{
		light->getIlluminationAtPoint(ray, diffuse, specular);
	}
	return {diffuse, specular};
}
