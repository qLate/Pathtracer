#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Color.h"
#include "Object.h"
#include "Action.h"

class Camera : public Object
{
public:
	inline static Camera* instance = nullptr;
	inline static Action onCameraMove {};
	inline static Action onCameraRotate {};

	float focalDistance, lensRadius;
	glm::vec2 size;
	Color bgColor;

	Camera(glm::vec3 pos, float focalDistance, float lensRadius, glm::vec2 size);

	void setBackgroundColor(Color color);

	void setRot(glm::quat rot) override;
	void setPos(glm::vec3 pos) override;

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
};
