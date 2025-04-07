#pragma once

#include "Color.h"
#include "Object.h"

class Light : public Object
{
	Color _color;
	float _intensity;

protected:
	Light(const glm::vec3& pos, const Color& color, float intensity);
	Light(const Light& orig);
	Light() = default;
	void init();

	~Light() override;

public:
	Color color() const { return _color; }
	float intensity() const { return _intensity; }

	void setPos(glm::vec3 pos, bool notify = true) override;
	void setRot(glm::quat rot, bool notify = true) override;

	void setColor(const Color& color);
	void setIntensity(float intensity);

	constexpr static auto properties();

private:
	void drawInspector() override { return LightInspectorDrawer::draw(this); }
	static Light* clone_internal(const Light* light) { return new Light(*light); }
};


class PointLight : public Light
{
	float _dis;

	PointLight() = default;

public:
	PointLight(glm::vec3 pos, Color color, float intensity, float dis);
	PointLight(const PointLight& orig);

	float dis() const { return _dis; }
	void setDis(float dis);

	constexpr static auto properties();

private:
	void drawInspector() override { return PointLightInspectorDrawer::draw(this); }
	PointLight* clone_internal() const override { return new PointLight(*this); }

	friend class JsonUtility;
};

class PointLightPBR : public PointLight
{
public:
	PointLightPBR(const glm::vec3& pos, const Color& color, float intensity);
};


class DirectionalLight : public Light
{
	DirectionalLight() = default;

public:
	DirectionalLight(glm::vec3 dir, Color color = Color::white(), float intensity = 1.0f);
	DirectionalLight(const DirectionalLight& orig);
	void init(glm::quat rot);

	constexpr static auto properties();

private:
	DirectionalLight* clone_internal() const override { return new DirectionalLight(*this); }

	friend class JsonUtility;
};

constexpr auto Light::properties()
{
	return std::tuple_cat(
		Object::properties(),
		std::make_tuple(
			JsonUtility::property(&Light::_color, "color"),
			JsonUtility::property(&Light::_intensity, "intensity")
		)
	);
}

constexpr auto PointLight::properties()
{
	return std::tuple_cat(
		Light::properties(),
		std::make_tuple(
			JsonUtility::property(&PointLight::_dis, "dis")
		)
	);
}

constexpr auto DirectionalLight::properties()
{
	return std::tuple_cat(
		Light::properties()
	);
}
