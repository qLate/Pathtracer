#include "ImGuiHandler.h"

#include "Camera.h"
#include "ImFileDialog.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "Renderer.h"
#include "SDLHandler.h"
#include "SDL_video.h"
#include "Utils.h"
#include "ImGuiExtensions.h"
#include "ImGUIWindowDrawer.h"

void ImGuiHandler::init()
{
	initImGui();
	initImFileDialog();
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

		return (void*)tex;
	};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex)
	{
		GLuint texID = (GLuint)tex;
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

	ImGUIWindowDrawer::drawMenuBar();
	ImGUIWindowDrawer::drawScene();
	ImGUIWindowDrawer::drawInspector();

	ImGui::Begin("Control Panel");
	if (ImGui::Button("Open file"))
		ifd::FileDialog::Instance().Open("ShaderOpenDialog", "Open a shader", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
	if (ImGui::Button("Open directory"))
		ifd::FileDialog::Instance().Open("DirectoryOpenDialog", "Open a directory", "");
	if (ImGui::Button("Save file"))
		ifd::FileDialog::Instance().Save("ShaderSaveDialog", "Save a shader", "*.sprj {.sprj}");
	ImGui::End();

	// file dialogs
	if (ifd::FileDialog::Instance().IsDone("ShaderOpenDialog"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
			for (const auto& r : res) // ShaderOpenDialog supports multiselection
				printf("OPEN[%s]\n", r.u8string().c_str());
		}
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("DirectoryOpenDialog"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			std::string res = ifd::FileDialog::Instance().GetResult().string();
			printf("DIRECTORY[%s]\n", res.c_str());
		}
		ifd::FileDialog::Instance().Close();
	}
	if (ifd::FileDialog::Instance().IsDone("ShaderSaveDialog"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			std::string res = ifd::FileDialog::Instance().GetResult().string();
			printf("SAVE[%s]\n", res.c_str());
		}
		ifd::FileDialog::Instance().Close();
	}

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
glm::vec2 ImGuiHandler::getRelativeMousePos(WindowType type)
{
	auto window = ImGui::FindWindowByType(type);
	if (!window) return {0, 0};

	auto pos = ImGui::GetMousePos();
	return glm::vec2(pos.x - window->Pos.x, pos.y - window->Pos.y);
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
