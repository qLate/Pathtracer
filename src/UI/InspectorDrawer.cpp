#include "InspectorDrawer.h"

#include "BufferController.h"
#include "Graphical.h"
#include "ImFileDialog.h"
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

		auto transform = target->getTransform();

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], matrixTranslation, matrixRotation, matrixScale);

		bool isDirty = false;
		if (ImGui::LabeledInputFloat3("Position", matrixTranslation)) isDirty = true;
		if (ImGui::LabeledInputFloat3("Rotation", matrixRotation)) isDirty = true;
		if (ImGui::LabeledInputFloat3("Scale", matrixScale)) isDirty = true;

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
		auto material = target->materialNoCopy();
		ImGui::LabeledInt("Material Id", material->id(), ImGuiInputTextFlags_ReadOnly);

		auto color = material->color();
		if (ImGui::LabeledColorEdit4("Color", &color[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float))
			material->setColor(color);

		auto diffuseCoef = material->diffuseCoef();
		if (ImGui::LabeledSliderFloat("Diffuse", diffuseCoef, 0.0f, 1.0f))
			material->setDiffuseCoef(diffuseCoef);

		if (target->sharedMaterial() == material && ImGui::Button("Clone Material"))
			material = target->material();

		if (ImGui::Button("Set Texture"))
		{
			auto dir = std::filesystem::current_path().concat("/assets/textures/").string();
			ifd::FileDialog::Instance().Open("Texture", "Open a texture file", "Image file (*.png;*.jpg;*.jpeg){.png,.jpg,.jpeg},.*", false, dir);
		}

		if (ifd::FileDialog::Instance().IsDone("Texture"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				auto path = ifd::FileDialog::Instance().GetResult().u8string();
				material->setTexture(new Texture(path));
			}
			ifd::FileDialog::Instance().Close();
		}
	}
	ImGui::PopID();
}
