#pragma once

#include "Camera.h"
#include "ImGuizmo.h"
#include "Utils.h"

class ObjectManipulator
{
	inline static constexpr glm::vec3 SNAP_TRANSLATION = glm::vec3(0.5f);
	inline static constexpr glm::vec3 SNAP_ROTATION = glm::vec3(45.f);
	inline static constexpr glm::vec3 SNAP_SCALE = glm::vec3(0.5f);

	inline static ImGuizmo::OPERATION _currGizmoOperation = ImGuizmo::TRANSLATE;
	inline static ImGuizmo::MODE _currGizmoMode = ImGuizmo::WORLD;

	inline static Object* _selectedObject = nullptr;
	inline static bool _isManipulating = false;

	static void update();

public:
	static void selectObject(Object* object);
	static void deselectObject();

	static Object* selectedObject() { return _selectedObject; }
	static bool isManipulating() { return _isManipulating; }

	static bool isMouseOverGizmo();

	friend class ImGuiHandler;
};
