#pragma once

#include "Camera.h"
#include "ImGuizmo.h"
#include "Utils.h"

class ObjectManipulator
{
	inline static constexpr glm::vec3 SNAP_TRANSLATION = glm::vec3(0.5f);
	inline static constexpr glm::vec3 SNAP_ROTATION = glm::vec3(45.f);
	inline static constexpr glm::vec3 SNAP_SCALE = glm::vec3(0.5f);

	inline static ImGuizmo::OPERATION _currGizmoOperation = ImGuizmo::ROTATE;
	inline static ImGuizmo::MODE _currGizmoMode = ImGuizmo::WORLD;

	inline static Object* _selectedObject = nullptr;

	static void update();

public:
	static void selectObject(Object* object);
	static void deselectObject();

	friend class ImGuiHandler;
};
