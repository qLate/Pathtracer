#pragma once

#include "Color.h"
#include "Object.h"
#include "Action.h"
#include "ImGUIHandler.h"

class Camera : public Object
{
public:
	inline static Camera* instance = nullptr;

	inline static Action onCameraMove {};
	inline static Action onCameraRotate {};

	glm::vec2 size;
	float focalDistance, lensRadius;
	Color bgColor;

	Camera(glm::vec3 pos, glm::vec2 size = {ImGUIHandler::INIT_RENDER_SIZE.x / (float)ImGUIHandler::INIT_RENDER_SIZE.y, 1}, float focalDistance = 1, float lensRadius = 0);

	void setBackgroundColor(Color color);

	void setRot(glm::quat rot) override;
	void setPos(glm::vec3 pos) override;
	void setSize(glm::vec2 size);

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
};
