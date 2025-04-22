#include "InspectorDrawer.h"

#include "Assets.h"
#include "Graphical.h"
#include "ImFileDialog.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Object.h"
#include "ImGuiExtensions.h"
#include "Light.h"
#include "ObjectManipulator.h"

void ObjectInspectorDrawer::draw(Object* target)
{
	const char* name = "Object";
	if (!ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) return;
	ImGui::PushID(name);
	{
		ImGui::LabeledInt("Object Id", target->id(), ImGuiInputTextFlags_ReadOnly);
		if (!target->name().empty())
			ImGui::LabeledText("Name", target->name().c_str(), ImGuiInputTextFlags_ReadOnly);

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

			auto startTransform = target->getTransform();
			target->setTransform(transform);
			ObjectManipulator::recordOperation(target, startTransform, transform);
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

		if (!material->texture()->name().empty())
			ImGui::LabeledText("Texture Name", material->texture()->name().c_str(), ImGuiInputTextFlags_ReadOnly);
		if (!material->texture()->path().empty())
			ImGui::LabeledText("Texture Path", material->texture()->path().c_str(), ImGuiInputTextFlags_ReadOnly);

		auto color = material->color();
		if (ImGui::LabeledColorEdit4("Color", &color[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float))
			material->setColor(color);

		auto roughness = material->roughness();
		if (ImGui::LabeledSliderFloat("Roughness", roughness, 0.0f, 1.0f))
			material->setDiffuseCoef(roughness);

		auto metallic = material->metallic();
		if (ImGui::LabeledSliderFloat("Metallic", metallic, 0.0f, 1.0f))
			material->setMetallic(metallic);

		auto specColor = material->specColor();
		if (ImGui::LabeledColorEdit4("Specular Color", &specColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float))
			material->setSpecColor(specColor);

		if (auto windy = dynamic_cast<WindyTexture*>(material->texture()))
		{
			auto scale = windy->scale();
			if (ImGui::LabeledSliderFloat("Windy Scale", scale, 0.0f, 1.0f))
				windy->setScale(scale);

			auto strength = windy->strength();
			if (ImGui::LabeledSliderFloat("Windy Strength", strength, 0.0f, 100.0f))
				windy->setStrength(strength);
		}

		auto emission = material->emission();
		if (ImGui::LabeledColorEdit4("Emission", &emission[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float))
			material->setEmission(emission);

		auto emissionIntensity = material->emission().intensity();
		if (ImGui::LabeledSliderFloat("Emission Intensity", emissionIntensity, 0, 20.0f))
			material->setEmission(emission.withIntensity(emissionIntensity));

		if (target->sharedMaterial() == material && ImGui::Button("Clone Material"))
			material = target->material();

		if (ImGui::Button("Set Texture"))
		{
			auto dir = std::filesystem::current_path().concat("/assets/textures/").string();
			ifd::FileDialog::Instance().Open("Texture", "Open a texture file", "Image file (*.png;*.jpg;*.jpeg;*.exr){.png,.jpg,.jpeg,.exr},.*", false, dir);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Texture"))
			material->setTexture(Texture::defaultTex());

		if (ifd::FileDialog::Instance().IsDone("Texture"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				auto path = ifd::FileDialog::Instance().GetResult().u8string();
				material->setTexture(Assets::load<Texture>(path));
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

void DiskInspectorDrawer::draw(Disk* target)
{
	GraphicalInspectorDrawer::draw(target);

	const char* name = "Disk";
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
		if (target->model() != nullptr)
			ImGui::LabeledInt("Triangle Count", target->model()->baseTriangles().size(), ImGuiInputTextFlags_ReadOnly);
		if (ImGui::Button("Set Model"))
		{
			auto dir = std::filesystem::current_path().concat("/assets/models/").string();
			ifd::FileDialog::Instance().Open("Model", "Open a model file", "Model file (*.obj){.obj},.*", false, dir);
		}

		//int c = 0;
		//for (const auto& tri : target->model()->baseTriangles())
		//{
		//	ImGui::PushID(c++);
		//	auto norm1 = target->getTransform() * glm::vec4(tri->vertices()[0].normal, 0);
		//	auto norm2 = target->getTransform() * glm::vec4(tri->vertices()[1].normal, 0);
		//	auto norm3 = target->getTransform() * glm::vec4(tri->vertices()[2].normal, 0);
		//	ImGui::LabeledInputFloat3("Vertex 0 Normal", &norm1.x, ImGuiInputTextFlags_ReadOnly);
		//	ImGui::LabeledInputFloat3("Vertex 1 Normal", &norm2.x, ImGuiInputTextFlags_ReadOnly);
		//	ImGui::LabeledInputFloat3("Vertex 2 Normal", &norm3.x, ImGuiInputTextFlags_ReadOnly);
		//	ImGui::PopID();
		//}

		if (ifd::FileDialog::Instance().IsDone("Model"))
		{
			if (ifd::FileDialog::Instance().HasResult())
			{
				auto path = ifd::FileDialog::Instance().GetResult().u8string();
				target->setModel(Assets::load<Model>(path));
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
