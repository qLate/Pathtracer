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

	void setColor(const Color& color);
	void setIntensity(float intensity);

	void drawInspector() override { return LightInspectorDrawer::draw(this); }
private:
};

class PointLight : public Light
{
	float _dis;

public:
	PointLight(glm::vec3 pos, Color color, float intensity, float dis);

	float dis() const { return _dis; }
	void setDis(float dis);

private:
	void drawInspector() override { return PointLightInspectorDrawer::draw(this); }
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 rot, Color color, float intensity);
};
