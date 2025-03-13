#pragma once

#include "Color.h"
#include "Object.h"
#include "ImGUIHandler.h"

class Camera : public Object
{
public:
	inline static Camera* instance = nullptr;

	glm::vec2 size;
	float focalDistance, lensRadius;
	Color bgColor = Color::black();

	Camera(glm::vec3 pos, glm::vec2 size = {ImGUIHandler::INIT_RENDER_SIZE.x / (float)ImGUIHandler::INIT_RENDER_SIZE.y, 1}, float focalDistance = 1, float lensRadius = 0);

	void setBackgroundColor(Color color);

	void setSize(glm::vec2 size);

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
	glm::vec3 getDir(const glm::vec2& screenPos) const;
	glm::vec3 getMouseDir() const;
};
