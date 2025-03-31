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

void WindowDrawer::init()
{
	_currRenderSize = ImGuiHandler::INIT_RENDER_SIZE;
}

void WindowDrawer::drawWindows()
{
	if (ImGui::IsKeyPressed(ImGuiKey_Tab))
		_showInspector = !_showInspector;

	if (ImGui::IsKeyPressed(ImGuiKey_F1))
		_showStats = !_showStats;

	if (ImGui::IsKeyPressed(ImGuiKey_F2))
		_showIcons = !_showIcons;

	if (!SDLHandler::isNavigatingScene() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
		SceneLoader::saveSceneDialog();
	if (!SDLHandler::isNavigatingScene() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O))
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
		ImGui::Image(Renderer::sceneViewFBO()->renderTexture()->id(), availSize, ImVec2(0, 1), ImVec2(1, 0));

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
	static float currFPS = -1;
	static float currVariance = -1;
	static float renderTime = -1;
	static float efficiency = -1;
	static int totalSamples = -1;
	static Timer updateTimer = Timer(100);
	static Timer slowUpdateTimer = Timer(500);

	if (updateTimer.trigger())
	{
		currFPS = ImGuiHandler::_io->Framerate;
		renderTime = Renderer::renderTime();
		totalSamples = Renderer::totalSamples();

		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
			Utils::copyToClipboard(std::format("{:.1f}ms, {:.3f} variance, {:.2f} efficiency", renderTime, currVariance, efficiency));
	}

	if (slowUpdateTimer.trigger() && Renderer::maxRayBounces() > 0)
	{
		currVariance = Renderer::computeSampleVariance();
		efficiency = 10 / currVariance / renderTime * Renderer::samplesPerPixel();
	}

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3fms)\n"
	            "%zu Triangles\n"
	            "Total samples: %d\n"
	            "Variance: %.3f (x1000)\n"
	            "Render time: %.3fms\n"
	            "Efficiency: %.3f\n",
	            currFPS, 1000.0f / currFPS,
	            Scene::triangles.size(),
	            totalSamples,
	            currVariance * 1000,
	            renderTime,
	            efficiency);
}

void WindowDrawer::drawInspector()
{
	ImGui::Begin("Inspector", nullptr);
	{
		if (auto obj = ObjectManipulator::selectedObject())
		{
			ImGui::PushID(obj->id());
			obj->drawInspector();
			ImGui::PopID();
		}
		else
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto moveSpeedMult = Input::_moveSpeedMult;
				if (ImGui::LabeledSliderFloat("Move Speed", moveSpeedMult, 0.1f, 20.0f))
					Input::_moveSpeedMult = moveSpeedMult;

				auto bgColor = Camera::instance->bgColor();
				if (ImGui::LabeledColorEdit3("Background Color", (float*)&bgColor, ImGuiColorEditFlags_NoInputs))
					Camera::instance->setBgColor(bgColor);
			}

			if (ImGui::CollapsingHeader("Path Tracing", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto limitSamples = Renderer::limitSamples();
				ImGui::LabeledCheckbox("Limit Samples", limitSamples);
				if (limitSamples != Renderer::limitSamples())
					Renderer::setLimitSamples(limitSamples);

				auto maxAccumSamples = Renderer::maxAccumSamples();
				ImGui::LabeledSliderInt("Max Accum Samples", maxAccumSamples, Renderer::samplesPerPixel(), 10000);
				if (maxAccumSamples != Renderer::maxAccumSamples())
					Renderer::setMaxAccumSamples(maxAccumSamples);

				auto samplesPerPixel = Renderer::samplesPerPixel();
				ImGui::LabeledSliderInt("Samples Per Pixel", samplesPerPixel, 1, 100);
				if (samplesPerPixel != Renderer::samplesPerPixel())
					Renderer::setSPP(samplesPerPixel);

				auto bounces = Renderer::maxRayBounces();
				ImGui::LabeledSliderInt("Ray Bounces", bounces, 0, 10);
				if (bounces != Renderer::maxRayBounces())
					Renderer::setMaxRayBounces(bounces);
			}
		}
	}
	ImGui::End();
}
