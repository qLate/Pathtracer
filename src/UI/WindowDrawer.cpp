#include "WindowDrawer.h"

#include "Camera.h"
#include "ImGuiExtensions.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "SDLHandler.h"
#include "Tweener.h"
#include "Utils.h"

void WindowDrawer::drawMenuBar()
{
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB))
		_showInspector = !_showInspector;

	if (Input::wasKeyPressed(SDL_SCANCODE_F1))
		_showStats = !_showStats;

	if (SDLHandler::isFullscreen()) return;

	ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(windowTypeToString(WindowType::Inspector), "Tab", _showInspector))
				_showInspector = !_showInspector;
			if (ImGui::MenuItem("Stats", "F1", _showStats))
				_showStats = !_showStats;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::PopItemFlag();
}

void WindowDrawer::drawScene()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	if (ImGuiHandler::_isAfterInit || SDLHandler::isMouseAttachedToScene())
		ImGui::SetNextWindowFocus();

	ImGui::Begin("Scene", nullptr);
	{
		auto node = ImGui::FindWindowByType(WindowType::Scene)->DockNode;
		if (!node->IsHiddenTabBar())
			node->WantHiddenTabBarToggle = true;

		ImVec2 availSize = ImGui::GetContentRegionAvail();
		ImGui::Image(Renderer::sceneViewFBO()->renderTexture->id(), availSize, ImVec2(0, 1), ImVec2(1, 0));

		if (availSize.x != _currRenderSize.x || availSize.y != _currRenderSize.y)
		{
			_currRenderSize = {availSize.x, availSize.y};
			Renderer::resizeView(_currRenderSize);
		}

		drawScene_displayStats(!node->IsHiddenTabBar());
	}
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void WindowDrawer::drawScene_displayStats(bool barVisible)
{
	if (!_showStats) return;

	static float currFPS;
	static Timer updateTimer = Timer(100);

	if (updateTimer.trigger())
		currFPS = ImGuiHandler::_io->Framerate;

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3f ms)\n%zu Triangles", currFPS, 1000.0f / currFPS, Scene::triangles.size());
}

void WindowDrawer::drawInspector()
{
	if (!_showInspector) return;

	auto moveSpeedMult = Input::_moveSpeedMult;
	auto bgColor = Camera::instance->bgColor();

	ImGui::Begin("Inspector", nullptr);
	{
		ImGui::SliderFloat("Move Speed", &moveSpeedMult, 0.1f, 20.0f);
		ImGui::ColorEdit3("Background Color", (float*)&bgColor, ImGuiColorEditFlags_NoInputs);
	}
	ImGui::End();

	Input::_moveSpeedMult = moveSpeedMult;
	if (bgColor != Camera::instance->bgColor()) Camera::instance->setBgColor(bgColor);
}
