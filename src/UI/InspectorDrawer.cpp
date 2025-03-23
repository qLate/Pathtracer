#include "InspectorDrawer.h"

#include "Assets.h"
#include "Graphical.h"
#include "ImFileDialog.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Object.h"
#include "ImGuiExtensions.h"
#include "Light.h"

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

		auto reflection = material->reflection();
		if (ImGui::LabeledSliderFloat("Reflection", reflection, 0.0f, 1.0f))
			material->setReflection(reflection);

		auto lit = material->lit();
		if (ImGui::LabeledCheckbox("Lit", lit))
			material->setLit(lit);

		if (target->sharedMaterial() == material && ImGui::Button("Clone Material"))
			material = target->material();

		if (ImGui::Button("Set Texture"))
		{
			auto dir = std::filesystem::current_path().concat("/assets/textures/").string();
			ifd::FileDialog::Instance().Open("Texture", "Open a texture file", "Image file (*.png;*.jpg;*.jpeg){.png,.jpg,.jpeg},.*", false, dir);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Texture"))
			material->setTexture(Texture::defaultTex());

		if (ifd::FileDialog::Instance().IsDone("Texture"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				auto path = ifd::FileDialog::Instance().GetResult().u8string();
				material->setTexture(Assets::import<Texture>(path));
			}
			ifd::FileDialog::Instance().Close();
		}
	}
	ImGui::PopID();
}
void SphereInspectorDrawer::draw(Sphere* target)
{
	GraphicalInspectorDrawer::draw(target);

	const char* name = "Sphere";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		auto radius = target->radius();
		if (ImGui::LabeledFloat("Radius", radius))
			target->setRadius(radius);
	}
	ImGui::PopID();
}

void MeshInspectorDrawer::draw(Mesh* target)
{
	GraphicalInspectorDrawer::draw(target);

	const char* name = "Mesh";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		auto triangles = target->triangles();
		ImGui::LabeledInt("Triangle Count", triangles.size(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::Button("Set Model"))
		{
			auto dir = std::filesystem::current_path().concat("/assets/models/").string();
			ifd::FileDialog::Instance().Open("Model", "Open a model file", "Model file (*.obj){.obj},.*", false, dir);
		}

		if (ifd::FileDialog::Instance().IsDone("Model"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				auto path = ifd::FileDialog::Instance().GetResult().u8string();
				target->setModel(Assets::import<Model>(path));
			}
			ifd::FileDialog::Instance().Close();
		}
	}
	ImGui::PopID();
}

void LightInspectorDrawer::draw(Light* target)
{
	ObjectInspectorDrawer::draw(target);

	const char* name = "Light";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		auto color = target->color();
		if (ImGui::LabeledColorEdit3("Color", &color[0], ImGuiColorEditFlags_NoInputs))
			target->setColor(color);

		auto intensity = target->intensity();
		if (ImGui::LabeledSliderFloat("Intensity", intensity, 0.0f, 5.0f))
			target->setIntensity(intensity);
	}
	ImGui::PopID();
}

void PointLightInspectorDrawer::draw(PointLight* target)
{
	LightInspectorDrawer::draw(target);

	const char* name = "Point Light";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		auto dis = target->dis();
		if (ImGui::LabeledFloat("Distance", dis))
			target->setDis(dis);
	}
	ImGui::PopID();
}
