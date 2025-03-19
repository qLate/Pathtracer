// ReSharper disable CppRedefinitionOfDefaultArgumentInOverrideFunction
#pragma once

#include "Color.h"
#include "Object.h"
#include "ImGUIHandler.h"

class Camera : public Object
{
	glm::vec2 _size;
	float _focalDis, _lensRadius;
	Color _bgColor = Color::black();

public:
	inline static Camera* instance = nullptr;

	Camera(glm::vec3 pos, glm::vec2 size = {ImGUIHandler::INIT_RENDER_SIZE.x / (float)ImGUIHandler::INIT_RENDER_SIZE.y, 1}, float focalDistance = 1, float lensRadius = 0);

	void setPos(glm::vec3 pos, bool notify = false) override;
	void setRot(glm::quat rot, bool notify = false) override;
	void setScale(glm::vec3 scale, bool notify = false) override;

	glm::vec2 size() const { return _size; }
	float focalDistance() const { return _focalDis; }
	float lensRadius() const { return _lensRadius; }
	Color bgColor() const { return _bgColor; }

	void setSize(glm::vec2 size);
	void setFocalDistance(float focalDistance);
	void setLensRadius(float lensRadius);
	void setBgColor(Color color);

	glm::vec3 getScreenCenter() const;
	glm::vec3 getLeftBotCorner() const;
	glm::vec3 getDir(const glm::vec2& screenPos) const;
	glm::vec3 getMouseDir() const;
};
