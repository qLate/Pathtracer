#pragma once

#include "Color.h"
#include "Object.h"

class Light : public Object
{
	Color _color;
	float _intensity;

protected:
	Light(const glm::vec3& pos, const Color& color, float intensity);

public:
	Color color() const { return _color; }
	float intensity() const { return _intensity; }

	void setColor(const Color& color) { _color = color; }
	void setIntensity(float intensity) { _intensity = intensity; }
};

class PointLight : public Light
{
	float _dis;

public:
	PointLight(glm::vec3 pos, Color color, float intensity, float dis);

	float dis() const { return _dis; }
	void setDis(float dis) { _dis = dis; }
};

class DirectionalLight : public Light
{
	glm::vec3 _dir;

public:
	DirectionalLight(glm::vec3 dir, Color color, float intensity);

	glm::vec3 dir() const { return _dir; }
	void setDir(glm::vec3 dir) { _dir = dir; }
};
