#include "InspectorDrawer.h"

#include "BufferController.h"
#include "Graphical.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Object.h"
#include "ImGuiExtensions.h"

void ObjectInspectorDrawer::draw(Object* target)
{
	const char* name = "Transform";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		ImGui::LabeledInt("Object Id", ((Graphical*)target)->indexId(), ImGuiInputTextFlags_ReadOnly);

		bool isDirty = false;
		auto transform = target->getTransform();

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], matrixTranslation, matrixRotation, matrixScale);

		ImGui::LabeledInputFloat3("Position", matrixTranslation, isDirty);
		ImGui::LabeledInputFloat3("Rotation", matrixRotation, isDirty);
		ImGui::LabeledInputFloat3("Scale", matrixScale, isDirty);

		if (isDirty)
		{
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &transform[0][0]);
			target->setTransform(transform);
		}
	}
	ImGui::PopID();
}

void GraphicalInspectorDrawer::draw(Graphical* target)
{
	ObjectInspectorDrawer::draw(target);

	const char* name = "Graphical";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		auto isDirty = false;
		auto material = target->material();

		ImGui::LabeledInt("Material Id", material->id(), isDirty, ImGuiInputTextFlags_ReadOnly);

		auto color = material->color();
		if (ImGui::LabeledColorEdit4("Color", &color[0], isDirty, ImGuiColorEditFlags_NoInputs))
			material->setColor(color);

		if (isDirty) BufferController::markBufferForUpdate(BufferType::Materials);
	}
	ImGui::PopID();
}
