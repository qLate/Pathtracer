#include "WindowDrawer.h"

#include "Camera.h"
#include "Graphical.h"
#include "IconDrawer.h"
#include "ImGuiExtensions.h"
#include "Input.h"
#include "Light.h"
#include "ObjectManipulator.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "SDLHandler.h"
#include "Utils.h"

void WindowDrawer::drawWindows()
{
	if (ImGui::IsKeyPressed(ImGuiKey_Tab))
		_showInspector = !_showInspector;

	if (ImGui::IsKeyPressed(ImGuiKey_F1))
		_showStats = !_showStats;

	if (ImGui::IsKeyPressed(ImGuiKey_F2))
		_showIcons = !_showIcons;

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
		SceneLoader::saveSceneDialog();
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O))
		SceneLoader::loadSceneDialog();

	drawMenuBar();
	drawScene();
	if (_showInspector) drawInspector();
}

void WindowDrawer::drawMenuBar()
{
	if (SDLHandler::isFullscreen()) return;

	ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SceneLoader::saveSceneDialog();
			if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
				SceneLoader::loadSceneDialog();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(windowTypeToString(WindowType::Inspector), "Tab", _showInspector))
				_showInspector = !_showInspector;
			if (ImGui::MenuItem("Stats", "F1", _showStats))
				_showStats = !_showStats;
			if (ImGui::MenuItem("Icons", "F2", _showIcons))
				_showIcons = !_showIcons;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Object"))
		{
			auto spawnPos = Camera::instance->pos() + Camera::instance->forward() * 10.0f;
			if (ImGui::MenuItem("New Mesh", "Tab"))
			{
				auto mesh = new Mesh(nullptr, spawnPos);
				ObjectManipulator::selectObject(mesh);
			}
			if (ImGui::MenuItem("New Sphere", "Tab"))
				ObjectManipulator::selectObject(new Sphere(spawnPos, 2));
			if (ImGui::MenuItem("New Cube", "Tab"))
				ObjectManipulator::selectObject(new Cube(spawnPos, 2));
			if (ImGui::MenuItem("New Square", "Tab"))
				ObjectManipulator::selectObject(new Square(spawnPos, 2));
			if (ImGui::MenuItem("New Plane", "Tab"))
				ObjectManipulator::selectObject(new Plane(spawnPos));
			if (ImGui::MenuItem("New Point Light", "Tab"))
				ObjectManipulator::selectObject(new PointLight(spawnPos, Color::white(), 1, 10));
			if (ImGui::MenuItem("New Directional Light", "Tab"))
				ObjectManipulator::selectObject(new DirectionalLight(spawnPos, Color::white(), 1));

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

	if (ImGuiHandler::_isAfterInit || SDLHandler::isNavigatingScene())
		ImGui::SetNextWindowFocus();

	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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

		if (_showIcons) IconDrawer::draw();
		if (_showStats) displayStats(!node->IsHiddenTabBar());
	}
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void WindowDrawer::displayStats(bool barVisible)
{
	static float currFPS;
	static Timer updateTimer = Timer(100);

	if (updateTimer.trigger())
		currFPS = ImGuiHandler::_io->Framerate;

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3f ms)\n%zu Triangles", currFPS, 1000.0f / currFPS, Scene::triangles.size());
}

void WindowDrawer::drawInspector()
{
	auto moveSpeedMult = Input::_moveSpeedMult;
	auto bgColor = Camera::instance->bgColor();

	ImGui::Begin("Inspector", nullptr);
	{
		if (auto obj = ObjectManipulator::selectedObject())
			obj->drawInspector();
		else
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SliderFloat("Move Speed", &moveSpeedMult, 0.1f, 20.0f);
				ImGui::ColorEdit3("Background Color", (float*)&bgColor, ImGuiColorEditFlags_NoInputs);
			}

			if (ImGui::CollapsingHeader("Path Tracing", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto bounces = Renderer::maxRayBounces();
				ImGui::SliderInt("Ray Bounces", &bounces, 1, 10);
				if (bounces != Renderer::maxRayBounces())
					Renderer::setMaxRayBounces(bounces);

				auto samplesPerPixel = Renderer::samplesPerPixel();
				ImGui::SliderInt("Samples Per Pixel", &samplesPerPixel, 1, 100);
				if (samplesPerPixel != Renderer::samplesPerPixel())
					Renderer::setSamplesPerPixel(samplesPerPixel);
			}
		}
	}
	ImGui::End();

	Input::_moveSpeedMult = moveSpeedMult;
	if (bgColor != Camera::instance->bgColor()) Camera::instance->setBgColor(bgColor);
}
