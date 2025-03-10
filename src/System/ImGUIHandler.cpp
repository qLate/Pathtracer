#include "ImGUIHandler.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "Pathtracer.h"
#include "Scene.h"
#include "SDLHandler.h"
#include "SDL_video.h"
#include "Utils.h"

void ImGUIHandler::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

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

void ImGUIHandler::tryInitInitialDocking()
{
	static bool init = false;
	if (init) return;
	init = true;

	auto dockSpace = ImGui::GetID("MyDockSpace");
	ImGuiID dock_id_left = 1, dock_id_right = 2;
	ImVec2 mainSize = ImGui::GetMainViewport()->Size;

	ImGui::DockBuilderRemoveNode(dockSpace);
	ImGui::DockBuilderAddNode(dockSpace);
	ImGui::DockBuilderSetNodeSize(dockSpace, mainSize);

	ImGui::DockBuilderSplitNode(dockSpace, ImGuiDir_Left, INSPECTOR_WIDTH / (float)RENDER_WIDTH, &dock_id_left, &dock_id_right);
	ImGui::DockBuilderDockWindow("Inspector", dock_id_left);
	ImGui::DockBuilderDockWindow("Scene", dock_id_right);

	ImGui::DockBuilderFinish(dockSpace);
}

void ImGUIHandler::update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	tryInitInitialDocking();

	ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpaceOverViewport(dockSpace_id);

	updateImGui_drawScene();
	updateImGui_drawInspector();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	updateImGui_finalizeViewports();
}

void ImGUIHandler::updateImGui_drawScene()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	ImGui::Begin("Scene");
	{
		auto node = ImGui::FindWindowByName("Scene")->DockNode;
		if (!node->IsHiddenTabBar())
			node->WantHiddenTabBarToggle = true;

		ImGui::Image(Pathtracer::sceneViewFBO->renderTexture->id, ImVec2(RENDER_WIDTH, RENDER_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

		updateImGui_drawScene_displayInfo(!node->IsHiddenTabBar());
	}
	ImGui::End();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}
void ImGUIHandler::updateImGui_drawScene_displayInfo(bool barVisible)
{
	static float currFPS;
	static Timer updateTimer = Timer(100);

	if (updateTimer.trigger())
		currFPS = io->Framerate;

	ImGui::SetCursorPos(ImVec2(5, 5 + (barVisible ? 20 : 0)));
	ImGui::Text("%.1f FPS (%.3f ms)\n%zu Triangles", currFPS, 1000.0f / currFPS, Scene::triangles.size());
}

void ImGUIHandler::updateImGui_drawInspector()
{
	auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Inspector");
	{
		ImGui::Text("This is some useful text.");

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
	}
	ImGui::End();
}

void ImGUIHandler::updateImGui_finalizeViewports()
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
