#include "ObjectManipulator.h"

#include "BufferController.h"
#include "BVH.h"
#include "SDLHandler.h"
#include "WindowDrawer.h"
#include "ImGUIHandler.h"

void ObjectManipulator::init()
{
	ImGuizmo::SetOrthographic(false);
}
void ObjectManipulator::update()
{
	updateManipulation();

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
		performUndo();
	else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Y))
		performRedo();
	else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_D, false))
		performDuplicate();
	else if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
		performDelete();
}
void ObjectManipulator::updateManipulation()
{
	if (!_selectedObject) return;

	ImGuizmo::BeginFrame();
	ImGuizmo::SetAlternativeWindow(ImGuiHandler::getWindow(WindowType::Scene));
	if (_lastUpdateSelectedObject == _selectedObject)
		ImGuizmo::Enable(true);
	else
		_lastUpdateSelectedObject = _selectedObject;

	updateChangeOperation();

	auto pos = ImGuiHandler::getWindowPos(WindowType::Scene);
	auto size = ImGuiHandler::getWindowSize(WindowType::Scene);
	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

	auto transform = _selectedObject->getTransform();
	auto viewMatrix = Camera::instance->getViewMatrix();
	auto projMatrix = Camera::instance->getProjectionMatrix();

	_useSnap = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
	auto snapValue = getSnapValue(_currGizmoOperation);

	if (Manipulate(&viewMatrix[0][0], &projMatrix[0][0], _currGizmoOperation, _currGizmoMode, &transform[0][0], NULL, _useSnap ? &snapValue.x : NULL))
	{
		if (!_isManipulating)
			_initialTransform = _selectedObject->getTransform();

		_isManipulating = true;
		_selectedObject->setTransform(transform);
	}

	if (_isManipulating && !ImGuizmo::IsUsing())
	{
		_isManipulating = false;
		recordOperation(_selectedObject, _initialTransform, transform);
	}
}
void ObjectManipulator::updateChangeOperation()
{
	if (!ImGuiHandler::isWindowFocused(WindowType::Scene) || SDLHandler::isNavigatingScene()) return;

	if (ImGui::IsKeyPressed(ImGuiKey_W, false))
		_currGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E, false))
		_currGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R, false))
		_currGizmoOperation = ImGuizmo::SCALE;
}

glm::vec3 ObjectManipulator::getSnapValue(ImGuizmo::OPERATION operation)
{
	switch (operation)
	{
	case ImGuizmo::TRANSLATE:
		return glm::vec3(SNAP_TRANSLATION);
	case ImGuizmo::ROTATE:
		return glm::vec3(SNAP_ROTATION);
	case ImGuizmo::SCALE:
		return glm::vec3(SNAP_SCALE);
	}
	throw std::runtime_error("Invalid gizmo operation.");
}

void ObjectManipulator::recordOperation(Object* object, const glm::mat4& transformFrom, const glm::mat4& transformTo)
{
	UndoRecord record = {object, transformFrom, transformTo};

	_currUndoIndex++;
	_undoRecords.resize(_currUndoIndex + 1);
	_undoRecords[_currUndoIndex] = record;
}
void ObjectManipulator::performUndo()
{
	if (_currUndoIndex < 0) return;

	auto& lastRecord = _undoRecords[_currUndoIndex];
	lastRecord.object->setTransform(lastRecord.transformFrom);
	_currUndoIndex--;
}
void ObjectManipulator::performRedo()
{
	if (_currUndoIndex >= (int)_undoRecords.size() - 1) return;

	auto& nextRecord = _undoRecords[_currUndoIndex + 1];
	nextRecord.object->setTransform(nextRecord.transformTo);
	_currUndoIndex++;
}

void ObjectManipulator::performDuplicate()
{
	if (!_selectedObject || SDLHandler::isNavigatingScene()) return;

	auto object = Object::clone(_selectedObject);
	object->setTransform(_selectedObject->getTransform());
	_selectedObject = object;

	BufferController::markBufferForUpdate(BufferType::Objects);
}
void ObjectManipulator::performDelete()
{
	if (!_selectedObject) return;

	Object::destroy(_selectedObject);
	_selectedObject = nullptr;

	BufferController::markBufferForUpdate(BufferType::Objects);
}

void ObjectManipulator::drawMenu()
{
	ImGui::Begin("Object Manipulation");

	if (ImGui::RadioButton("Translate", _currGizmoOperation == ImGuizmo::TRANSLATE))
		_currGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", _currGizmoOperation == ImGuizmo::ROTATE))
		_currGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", _currGizmoOperation == ImGuizmo::SCALE))
		_currGizmoOperation = ImGuizmo::SCALE;

	if (_currGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", _currGizmoMode == ImGuizmo::LOCAL))
			_currGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", _currGizmoMode == ImGuizmo::WORLD))
			_currGizmoMode = ImGuizmo::WORLD;
	}

	ImGui::End();
}

void ObjectManipulator::selectObject(Object* object)
{
	_selectedObject = object;
	ImGuizmo::Enable(false);
}
void ObjectManipulator::deselectObject()
{
	_selectedObject = nullptr;
	_lastUpdateSelectedObject = nullptr;
}

bool ObjectManipulator::isMouseOverGizmo()
{
	return isDisplaying() && ImGuizmo::IsOver();
}
