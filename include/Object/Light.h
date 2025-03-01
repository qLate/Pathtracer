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
	float distance;

	PointLight(glm::vec3 pos, Color color, float intensity, float distance);
};

class AreaLight : public Light
{
public:
	float distance;
	glm::vec3 size;
	glm::vec3 pointSize;

	AreaLight(glm::vec3 pos, Color color, float intensity, float distance, glm::vec3 size, glm::vec3 pointSize);
};

class GlobalLight : public Light
{
public:
	glm::vec3 direction;

	GlobalLight(glm::vec3 direction, Color color, float intensity);
};

class EverywhereLight : public Light
{
public:
	EverywhereLight(const Color& color, float intensity);
};

