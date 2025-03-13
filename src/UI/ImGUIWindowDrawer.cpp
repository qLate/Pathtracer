#include "ImGUIWindowDrawer.h"

#include "Camera.h"
#include "ImGuiExtensions.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "SDLHandler.h"
#include "Tweener.h"
#include "Utils.h"

void ImGUIWindowDrawer::drawMenuBar()
{
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB))
		showInspector = !showInspector;

	if (Input::wasKeyPressed(SDL_SCANCODE_F1))
		showStats = !showStats;

	if (SDLHandler::isFullscreen) return;

	ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(windowTypeToString(WindowType::Inspector), "Tab", showInspector))
				showInspector = !showInspector;
			if (ImGui::MenuItem("Stats", "F1", showStats))
				showStats = !showStats;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::PopItemFlag();
}

void ImGUIWindowDrawer::drawScene()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	if (ImGUIHandler::isAfterInit || SDLHandler::mouseAttachedToScene)
		ImGui::SetNextWindowFocus();

	ImGui::Begin("Scene", nullptr);
	{
		auto node = ImGui::FindWindowByType(WindowType::Scene)->DockNode;
		if (!node->IsHiddenTabBar())
			node->WantHiddenTabBarToggle = true;

		ImVec2 availSize = ImGui::GetContentRegionAvail();
		ImGui::Image(Renderer::sceneViewFBO->renderTexture->id, availSize, ImVec2(0, 1), ImVec2(1, 0));

		if (availSize.x != currRenderSize.x || availSize.y != currRenderSize.y)
		{
			currRenderSize = {availSize.x, availSize.y};
			Renderer::resizeView(currRenderSize);
		}

		drawScene_displayStats(!node->IsHiddenTabBar());
	}
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void ImGUIWindowDrawer::drawScene_displayStats(bool barVisible)
{
	if (!showStats) return;

	static float currFPS;
	static Timer updateTimer = Timer(100);

	if (updateTimer.trigger())
		currFPS = ImGUIHandler::io->Framerate;

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3f ms)\n%zu Triangles", currFPS, 1000.0f / currFPS, Scene::triangles.size());
}

void ImGUIWindowDrawer::drawInspector()
{
	if (!showInspector) return;

	auto moveSpeedMult = Input::moveSpeedMult;
	auto bgColor = Camera::instance->bgColor;

	ImGui::Begin("Inspector", nullptr);
	{
		ImGui::SliderFloat("Move Speed", &moveSpeedMult, 0.1f, 20.0f);
		ImGui::ColorEdit3("Background Color", (float*)&bgColor, ImGuiColorEditFlags_NoInputs);
	}
	ImGui::End();

	Input::moveSpeedMult = moveSpeedMult;
	if (bgColor != Camera::instance->bgColor) Camera::instance->setBackgroundColor(bgColor);
}
