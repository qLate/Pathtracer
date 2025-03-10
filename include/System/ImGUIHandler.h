#pragma once

#include "imgui.h"
#include "SDL_egl.h"
#include "glm/vec2.hpp"

class ImGUIHandler
{
	static constexpr char GLSL_VERSION[] = "#version 460";
	static constexpr int INSPECTOR_WIDTH = 300;

public:
	static constexpr int RENDER_WIDTH = 640 * 2;
	static constexpr int RENDER_HEIGHT = 360 * 2;
	static constexpr glm::ivec2 RENDER_SIZE = {RENDER_WIDTH, RENDER_HEIGHT};
	static constexpr glm::vec2 RENDER_RATIO = {RENDER_WIDTH / (float)RENDER_HEIGHT, 1};

	inline static ImGuiIO* io;

	static constexpr glm::ivec2 calculateRequiredWindowSize()
	{
		return { RENDER_WIDTH + INSPECTOR_WIDTH, RENDER_HEIGHT };
	}

	static void init();
	static void tryInitInitialDocking();

	static void update();
	static void updateImGui_drawScene();
	static void updateImGui_drawScene_displayInfo(bool barVisible);
	static void updateImGui_drawInspector();
	static void updateImGui_finalizeViewports();
};
