#pragma once

#include "Camera.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Utils.h"

class ObjectManipulator
{
	struct UndoRecord
	{
		Object* object;
		glm::mat4 transformFrom;
		glm::mat4 transformTo;
	};

	inline static constexpr glm::vec3 SNAP_TRANSLATION = glm::vec3(2.0f);
	inline static constexpr glm::vec3 SNAP_ROTATION = glm::vec3(45.f);
	inline static constexpr glm::vec3 SNAP_SCALE = glm::vec3(0.5f);

	inline static ImGuizmo::OPERATION _currGizmoOperation = ImGuizmo::TRANSLATE;
	inline static ImGuizmo::MODE _currGizmoMode = ImGuizmo::WORLD;
	inline static bool _useSnap = false;

	inline static Object* _selectedObject = nullptr;
	inline static Object* _lastUpdateSelectedObject = nullptr;
	inline static bool _isManipulating = false;

	inline static glm::mat4x4 _initialTransform;
	inline static std::vector<UndoRecord> _undoRecords;
	inline static int _currUndoIndex = -1;

	static void init();

	static void update();

	static void updateManipulation();
	static void updateChangeOperation();
	static glm::vec3 getSnapValue(ImGuizmo::OPERATION operation);


	static void performDuplicate();
	static void performDelete();

	static void drawMenu();

public:
	static void selectObject(Object* object);
	static void deselectObject();

	static Object* selectedObject() { return _selectedObject; }
	static bool isDisplaying() { return _selectedObject != nullptr; }
	static bool isManipulating() { return _isManipulating; }

	static bool isMouseOverGizmo();

	static void recordOperation(Object* object, const glm::mat4& transformFrom, const glm::mat4& transformTo);
	static void performUndo();
	static void performRedo();

	friend class ImGuiHandler;
};
