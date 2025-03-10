#pragma once

#include "imgui.h"
#include "SDL_egl.h"
#include "glm/vec2.hpp"

class ImGUIHandler
{
	static constexpr char GLSL_VERSION[] = "#version 460";
	static constexpr int INSPECTOR_WIDTH = 300;

	inline static bool init = true;

public:
	static constexpr glm::ivec2 INIT_RENDER_SIZE = {640 * 2, 360 * 2};
	static constexpr glm::ivec2 INIT_FULL_WINDOW_SIZE = {INIT_RENDER_SIZE.x, INIT_RENDER_SIZE.y};

	inline static glm::ivec2 currRenderSize = INIT_RENDER_SIZE;

	inline static ImGuiIO* io;

	inline static bool showInspector = true;

	static void initialize();
	static void initDocking();

	static void update();
	static void updateDocking();

	static void drawMenuBar();

	static void drawScene();
	static void drawScene_displayInfo(bool barVisible);
	static void drawInspector();

	static void finalizeViewports();
};
