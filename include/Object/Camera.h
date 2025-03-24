#pragma once

#include "Color.h"
#include "Object.h"
#include "ImGUIHandler.h"

class Camera : public Object
{
	glm::vec2 _ratio;
	float _focalDis, _lensRadius;
	Color _bgColor = Color::black();

	Camera() = default;

public:
	inline static Camera* instance = nullptr;

	Camera(glm::vec3 pos, float focalDistance = 1, float lensRadius = 0);
	Camera(const Camera& orig);
	~Camera() override;

	void setPos(glm::vec3 pos, bool notify = true) override;
	void setRot(glm::quat rot, bool notify = true) override;
	void setScale(glm::vec3 scale, bool notify = true) override;

	glm::vec2 ratio() const { return _ratio; }
	float focalDistance() const { return _focalDis; }
	float lensRadius() const { return _lensRadius; }
	Color bgColor() const { return _bgColor; }

	void setRatio(glm::vec2 ratio);
	void setFocalDistance(float focalDistance);
	void setLensRadius(float lensRadius);
	void setBgColor(Color color);

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
	glm::vec3 getDir(const glm::vec2& screenPos) const;
	glm::vec3 getMouseDir() const;

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	glm::vec2 worldToViewportPos(const glm::vec3& worldPos) const;
	glm::ivec2 worldToScreenPos(const glm::vec3& worldPos) const;

	constexpr static auto properties()
	{
		return std::tuple_cat(
			Object::properties(),
			std::make_tuple(
				JsonUtility::property(&Camera::_focalDis, "focalDistance"),
				JsonUtility::property(&Camera::_lensRadius, "lensRadius"),
				JsonUtility::property(&Camera::_bgColor, "bgColor")
			)
		);
	}

	friend class JsonUtility;
};
