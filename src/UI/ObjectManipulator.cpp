#include "ObjectManipulator.h"

#include "BufferController.h"
#include "BVH.h"
#include "ImGuizmo.h"
#include "SDLHandler.h"
#include "WindowDrawer.h"

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
}
void ObjectManipulator::updateManipulation()
{
	if (!_selectedObject) return;

	ImGuizmo::BeginFrame();
	ImGuizmo::SetAlternativeWindow(ImGuiHandler::getWindow(WindowType::Scene));

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

	if (ImGui::IsKeyPressed(ImGuiKey_W))
		_currGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		_currGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
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

void ObjectManipulator::drawMenu()
{
	ImGui::Begin("Object Manipulation");
	if (!_selectedObject)
	{
		ImGui::End();
		return;
	}

	if (ImGui::RadioButton("Translate", _currGizmoOperation == ImGuizmo::TRANSLATE))
		_currGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", _currGizmoOperation == ImGuizmo::ROTATE))
		_currGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", _currGizmoOperation == ImGuizmo::SCALE))
		_currGizmoOperation = ImGuizmo::SCALE;

	auto transform = _selectedObject->getTransform();

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &transform[0][0]);

	_selectedObject->setTransform(transform);

	if (_currGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", _currGizmoMode == ImGuizmo::LOCAL))
			_currGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", _currGizmoMode == ImGuizmo::WORLD))
			_currGizmoMode = ImGuizmo::WORLD;
	}
}

void ObjectManipulator::selectObject(Object* object)
{
	_selectedObject = object;
}
void ObjectManipulator::deselectObject()
{
	_selectedObject = nullptr;
}

bool ObjectManipulator::isMouseOverGizmo()
{
	return ImGuizmo::IsOver();
}
