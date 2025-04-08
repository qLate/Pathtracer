#pragma once

#include "Color.h"
#include "Object.h"

class Camera : public Object
{
	float _ratio;
	float _fov = 53.13f, _lensRadius;
	Color _bgColor = Color::black();

	float _pitch = 0, _yaw = 0, _roll = 0;

	Camera() = default;

public:
	inline static Camera* instance = nullptr;

	Camera(glm::vec3 pos, float fov = 53.13f, float lensRadius = 0);
	Camera(const Camera& orig);
	void init();

	~Camera() override;

	void setPos(glm::vec3 pos, bool notify = true) override;
	void setRot(glm::quat rot, bool notify = true) override;
	void setRot(float pitch, float yaw, float roll = -1);

	float pitch() const { return _pitch; }
    float yaw() const { return _yaw; }
	float roll() const { return _roll; }
	float ratio() const { return _ratio; }
	float fov() const { return _fov; }
	float lensRadius() const { return _lensRadius; }
	Color bgColor() const { return _bgColor; }

	void setRatio(float ratio);
	void setFov(float fov);
	void setLensRadius(float lensRadius);
	void setBgColor(Color color);

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
	glm::vec3 getDir(const glm::vec2& screenPos) const;
	glm::vec3 getMouseDir() const;

	float getFocalDis() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	glm::vec2 worldToViewportPos(const glm::vec3& worldPos) const;
	glm::ivec2 worldToScreenPos(const glm::vec3& worldPos) const;

	constexpr static auto properties()
	{
		return std::tuple_cat(
			Object::properties(),
			std::make_tuple(
				JsonUtility::property(&Camera::_fov, "fov"),
				JsonUtility::property(&Camera::_lensRadius, "lensRadius"),
				JsonUtility::property(&Camera::_bgColor, "bgColor")
			)
		);
	}

	friend class JsonUtility;
};
