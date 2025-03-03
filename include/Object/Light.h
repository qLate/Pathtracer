#pragma once

#include "Color.h"
#include "Object.h"

class Light : public Object
{
protected:
	Light(const glm::vec3& pos, const Color& color, float intensity);

public:
	Color color;
	float intensity;
};

class PointLight : public Light
{
public:
	float dis;

	PointLight(glm::vec3 pos, Color color, float intensity, float dis);
};

class DirectionalLight : public Light
{
public:
	glm::vec3 dir;

	DirectionalLight(glm::vec3 dir, Color color, float intensity);
};
