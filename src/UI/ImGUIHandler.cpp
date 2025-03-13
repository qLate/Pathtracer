#include "ImGUIHandler.h"

#include "Camera.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "Renderer.h"
#include "SDLHandler.h"
#include "SDL_video.h"
#include "Utils.h"
#include "ImGuiExtensions.h"
#include "ImGUIWindowDrawer.h"

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
	ImGui::DockBuilderDockWindow(windowTypeToString(WindowType::Inspector), dock_id_left);
	ImGui::DockBuilderDockWindow(windowTypeToString(WindowType::Scene), dock_id_right);

	ImGui::DockBuilderFinish(dockSpace);
}

void ImGUIHandler::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (isInit) initDocking();
	updateDocking();

	ImGUIWindowDrawer::drawMenuBar();
	ImGUIWindowDrawer::drawScene();
	ImGUIWindowDrawer::drawInspector();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	finalizeViewports();

	if (isInit)
	{
		isInit = false;
		isAfterInit = true;
	}
	else if (isAfterInit)
		isAfterInit = false;
}
void ImGUIHandler::updateDocking()
{
	ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpaceOverViewport(dockSpace_id);
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

bool ImGUIHandler::isWindowFocused(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	return window && window->DockNode && window->DockNode->IsFocused;
}
const char* windowTypeToString(WindowType type)
{
	switch (type)
	{
	case WindowType::Scene:
		return "Scene";
	case WindowType::Inspector:
		return "Inspector";
	}
	throw std::runtime_error("Invalid WindowType");
}
