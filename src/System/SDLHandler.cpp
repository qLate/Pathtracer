#include "SDLHandler.h"

#include <iostream>

#include "glad.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "Input.h"
#include "Pathtracer.h"

void SDLHandler::initialize()
{
	SDL_GL_LoadLibrary(nullptr);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	window = SDL_CreateWindow("Pathtracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W_WIDTH, W_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	initImGUI();
	initOpenGL();
}
void SDLHandler::initImGUI()
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
	}

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	io->Fonts->AddFontFromFileTTF("assets/fonts/Cousine-Regular.ttf", 15.0f);
}
void SDLHandler::initOpenGL()
{
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//sceneViewFBO = new GLFrameBuffer(W_WIDTH, W_HEIGHT);
}

void SDLHandler::updateDrawScene()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFBO->id);

	Pathtracer::traceShaderP->use();
	glBindVertexArray(Pathtracer::traceShaderP->fragShader->vaoScreen->id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SDLHandler::updateImGui()
{
	auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
	//ImGui::DockSpaceOverViewport(dockSpace_id);

	//ImGui::Begin("image");
	//ImGui::Image(sceneViewFBO->renderTexture->id, ImVec2(W_WIDTH, W_HEIGHT));
	//ImGui::End();

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

bool SDLHandler::updateEvents()
{
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		Input::handleSDLEvent(event);

		if (event.type == SDL_QUIT) return false;
	}
	return true;
}

void SDLHandler::quit()
{
	//delete sceneViewFBO;

	SDL_DestroyWindow(window);
	SDL_Quit();
}
