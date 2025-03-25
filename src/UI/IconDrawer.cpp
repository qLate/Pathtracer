#include "IconDrawer.h"

#include "Assets.h"
#include "Camera.h"
#include "ImGuiHandler.h"
#include "Light.h"
#include "Material.h"
#include "Scene.h"
#include "WindowDrawer.h"

void IconDrawer::init()
{
	_lightIconTex = Assets::load<Texture>("assets/textures/core/lightIcon.png");
}

void IconDrawer::draw()
{
	for (auto& light : Scene::lights)
	{
		auto pos = light->pos();
		auto screenPos = Camera::instance->worldToScreenPos(pos);
		if (screenPos.x < 0 || screenPos.x > WindowDrawer::currRenderSize().x || screenPos.y < 0 || screenPos.y > WindowDrawer::currRenderSize().y) continue;

		auto lt = ImVec2(screenPos.x - ICON_SIZE / 2, screenPos.y - ICON_SIZE / 2);
		ImGui::SetCursorPos(lt);
		ImGui::Image(_lightIconTex->glTex()->id(), ImVec2(ICON_SIZE, ICON_SIZE));
	}
}
