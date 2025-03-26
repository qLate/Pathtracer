#include "ImGuiHandler.h"

#include "Camera.h"
#include "IconDrawer.h"
#include "ImFileDialog.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "Renderer.h"
#include "SDLHandler.h"
#include "SDL_video.h"
#include "Utils.h"
#include "ImGuiExtensions.h"
#include "ObjectManipulator.h"
#include "SceneLoader.h"
#include "WindowDrawer.h"

void ImGuiHandler::init()
{
	initImGui();
	initImFileDialog();
	WindowDrawer::init();
	ObjectManipulator::init();
	IconDrawer::init();
}
void ImGuiHandler::initImFileDialog()
{
	ifd::FileDialog::Instance().CreateTexture = [](const uint8_t* data, int w, int h, char fmt) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, fmt == 0 ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return reinterpret_cast<void*>(static_cast<uintptr_t>(tex));
	};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex)
	{
			GLuint texID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(tex));
		glDeleteTextures(1, &texID);
	};
}
void ImGuiHandler::initImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	_io = &ImGui::GetIO();
	_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	_io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

	ImGuiStyle& style = ImGui::GetStyle();
	if (_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.FrameBorderSize = 0;
	}

	ImGui_ImplSDL2_InitForOpenGL(SDLHandler::window(), SDLHandler::context());
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	_io->Fonts->AddFontFromFileTTF("assets/fonts/Cousine-Regular.ttf", 15.0f);
}
void ImGuiHandler::initDocking()
{
	auto dockSpace = ImGui::GetID("MyDockSpace");
	ImGuiID dock_id_left = 1, dock_id_right = 2;

	ImGui::DockBuilderRemoveNode(dockSpace);
	ImGui::DockBuilderAddNode(dockSpace);
	ImGui::DockBuilderSetNodeSize(dockSpace, ImGui::GetMainViewport()->Size);

	ImGui::DockBuilderSplitNode(dockSpace, ImGuiDir_Left, INSPECTOR_WIDTH / (float)INIT_FULL_WINDOW_SIZE.x, &dock_id_left, &dock_id_right);
	ImGui::DockBuilderDockWindow(windowTypeToString(WindowType::Inspector), dock_id_left);
	ImGui::DockBuilderDockWindow(windowTypeToString(WindowType::Scene), dock_id_right);

	ImGui::DockBuilderFinish(dockSpace);
}

void ImGuiHandler::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (_isInit) initDocking();
	updateDocking();

	SceneLoader::update();
	WindowDrawer::drawWindows();
	ObjectManipulator::update();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	finalizeViewports();

	if (_isInit)
	{
		_isInit = false;
		_isAfterInit = true;
	}
	else if (_isAfterInit)
		_isAfterInit = false;
}
void ImGuiHandler::updateDocking()
{
	ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpaceOverViewport(dockSpace_id);
}

void ImGuiHandler::finalizeViewports()
{
	if (_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

bool ImGuiHandler::isWindowFocused(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	return window && window->DockNode && window->DockNode->IsFocused;
}
bool ImGuiHandler::isWindowHovered(WindowType type)
{
	ImGuiWindow* window;
	ImGui::FindHoveredWindowEx(ImGui::GetMousePos(), true, &window, nullptr);
	return getWindow(type) == window;
}

ImGuiWindow* ImGuiHandler::getWindow(WindowType type)
{
	return ImGui::FindWindowByType(type);
}
glm::vec2 ImGuiHandler::getWindowPos(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	if (!window) return {0, 0};
	return {window->Pos.x, window->Pos.y};
}
glm::vec2 ImGuiHandler::getWindowSize(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	if (!window) return {0, 0};
	return {window->Size.x, window->Size.y};
}
glm::vec2 ImGuiHandler::getRelativeMousePos(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	if (!window) return {0, 0};

	auto pos = ImGui::GetMousePos();
	return glm::vec2(pos.x - window->Pos.x, pos.y - window->Pos.y);
}

void ImGuiHandler::drawDebugBox(const glm::vec2& pos, const glm::vec2& size)
{
	ImGui::Begin("Debug");
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(255, 0, 0, 255));
	ImGui::End();
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
