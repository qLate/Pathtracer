#include "ObjectManipulator.h"

#include "BufferController.h"
#include "ImGuizmo.h"
#include "WindowDrawer.h"

void ObjectManipulator::update()
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetAlternativeWindow(ImGuiHandler::getWindow(WindowType::Scene));

	ImGui::Begin("Object Manipulation");

	if (!_selectedObject)
	{
		ImGui::Text("No object selected.");
		ImGui::End();
		return;
	}
	auto transform = _selectedObject->getTransform();

	if (ImGuiHandler::isWindowFocused(WindowType::Scene))
	{
		if (ImGui::IsKeyPressed(ImGuiKey_W))
			_currGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			_currGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R))
			_currGizmoOperation = ImGuizmo::SCALE;
	}

	if (ImGui::RadioButton("Translate", _currGizmoOperation == ImGuizmo::TRANSLATE))
		_currGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", _currGizmoOperation == ImGuizmo::ROTATE))
		_currGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", _currGizmoOperation == ImGuizmo::SCALE))
		_currGizmoOperation = ImGuizmo::SCALE;

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &transform[0][0]);

	if (_currGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", _currGizmoMode == ImGuizmo::LOCAL))
			_currGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", _currGizmoMode == ImGuizmo::WORLD))
			_currGizmoMode = ImGuizmo::WORLD;
	}

	static bool useSnap(false);
	if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl))
		useSnap = !useSnap;
	ImGui::Checkbox("##", &useSnap);
	ImGui::SameLine();

	glm::vec3 snap;
	switch (_currGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = glm::vec3(SNAP_TRANSLATION);
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap = glm::vec3(SNAP_ROTATION);
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap = glm::vec3(SNAP_SCALE);
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	auto viewMatrix = Camera::instance->getViewMatrix();
	auto projMatrix = Camera::instance->getProjectionMatrix();

	auto pos = ImGuiHandler::getWindowPos(WindowType::Scene);
	auto size = ImGuiHandler::getWindowSize(WindowType::Scene);
	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

	if (Manipulate(&viewMatrix[0][0], &projMatrix[0][0], _currGizmoOperation, _currGizmoMode, &transform[0][0], NULL, useSnap ? &snap.x : NULL))
	{
		_isManipulating = true;
		_selectedObject->setTransform(transform);
	}
	else
		_isManipulating = false;

	ImGui::End();
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
