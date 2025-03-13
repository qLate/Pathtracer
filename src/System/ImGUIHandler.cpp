#include "ImGUIHandler.h"

#include "Camera.h"
#include "GLObject.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "Input.h"
#include "Renderer.h"
#include "Scene.h"
#include "SDLHandler.h"
#include "SDL_video.h"
#include "Utils.h"

void ImGUIHandler::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.FrameBorderSize = 0;
	}

	ImGui_ImplSDL2_InitForOpenGL(SDLHandler::window, SDLHandler::context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	io->Fonts->AddFontFromFileTTF("assets/fonts/Cousine-Regular.ttf", 15.0f);
}

void ImGUIHandler::initDocking()
{
	auto dockSpace = ImGui::GetID("MyDockSpace");
	ImGuiID dock_id_left = 1, dock_id_right = 2;
	ImVec2 mainSize = ImGui::GetMainViewport()->Size;

	ImGui::DockBuilderRemoveNode(dockSpace);
	ImGui::DockBuilderAddNode(dockSpace);
	ImGui::DockBuilderSetNodeSize(dockSpace, mainSize);

	ImGui::DockBuilderSplitNode(dockSpace, ImGuiDir_Left, INSPECTOR_WIDTH / (float)INIT_FULL_WINDOW_SIZE.x, &dock_id_left, &dock_id_right);
	ImGui::DockBuilderDockWindow("Inspector", dock_id_left);
	ImGui::DockBuilderDockWindow("Scene", dock_id_right);

	ImGui::DockBuilderFinish(dockSpace);
}

void ImGUIHandler::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (isInit) initDocking();
	updateDocking();

	drawMenuBar();
	drawScene();
	drawInspector();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	finalizeViewports();

	if (isInit) isInit = false;
}
void ImGUIHandler::updateDocking()
{
	ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpaceOverViewport(dockSpace_id);
}

void ImGUIHandler::drawMenuBar()
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
			if (ImGui::MenuItem("Inspector", "Tab", showInspector))
				showInspector = !showInspector;
			if (ImGui::MenuItem("Stats", "F1", showStats))
				showStats = !showStats;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::PopItemFlag();
}

void ImGUIHandler::drawScene()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE) && SDLHandler::sceneFocused)
		ImGui::SetNextWindowFocus();

	ImGui::Begin("Scene", nullptr);
	{
		auto node = ImGui::FindWindowByName("Scene")->DockNode;
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
void ImGUIHandler::drawScene_displayStats(bool barVisible)
{
	if (!showStats) return;

	static float currFPS;
	static Timer updateTimer = Timer(100);

	if (updateTimer.trigger())
		currFPS = io->Framerate;

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3f ms)\n%zu Triangles", currFPS, 1000.0f / currFPS, Scene::triangles.size());
}
void ImGUIHandler::drawInspector()
{
	if (!showInspector) return;

	auto bgColor = Camera::instance->bgColor;
	auto moveSpeedMult = Input::moveSpeedMult;

	ImGui::Begin("Inspector", nullptr);
	{
		ImGui::ColorEdit3("Background Color", (float*)&bgColor, ImGuiColorEditFlags_NoInputs);
		ImGui::SliderFloat("Move Speed", &moveSpeedMult, 0.1f, 20.0f);
	}
	ImGui::End();

	if (bgColor != Camera::instance->bgColor)
		Camera::instance->setBackgroundColor(bgColor);

	Input::moveSpeedMult = moveSpeedMult;
}

void ImGUIHandler::finalizeViewports()
{
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}
