#include "InspectorDrawer.h"

#include "BufferController.h"
#include "Graphical.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Object.h"
#include "ImGuiExtensions.h"

void BaseInspectorDrawer::draw(Object* target)
{
	drawInnerWrapper(target);
}

void ObjectInspectorDrawer::drawInner(Object* target)
{
	auto isDirty = false;
	auto transform = target->getTransform();

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], matrixTranslation, matrixRotation, matrixScale);

	if (ImGui::LabeledInputFloat3("Position", matrixTranslation)) isDirty = true;
	if (ImGui::LabeledInputFloat3("Rotation", matrixRotation)) isDirty = true;
	if (ImGui::LabeledInputFloat3("Scale", matrixScale)) isDirty = true;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &transform[0][0]);

	if (isDirty) target->setTransform(transform);
}
void ObjectInspectorDrawer::drawInnerWrapper(Object* target)
{
	const char* name = "Object";
	if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID(name);
		drawInner(target);
		ImGui::PopID();
	}
}

void GraphicalInspectorDrawer::drawInner(Graphical* target)
{
	auto isDirty = false;

	auto material = target->material();
	auto color = material->color();
	if (ImGui::LabeledColorEdit4("Color", &color[0], ImGuiColorEditFlags_NoInputs))
	{
		isDirty = true;
		material->setColor(color);
	}

	if (isDirty)
		BufferController::markBufferForUpdate(BufferType::Materials);
}
void GraphicalInspectorDrawer::drawInnerWrapper(Object* target)
{
	ObjectInspectorDrawer::drawInnerWrapper(target);

	const char* name = "Graphical";
	if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID(name);
		drawInner(dynamic_cast<Graphical*>(target));
		ImGui::PopID();
	}
}
