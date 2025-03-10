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
	static constexpr glm::ivec2 calculateInitialFullWindowSize();

	inline static glm::ivec2 currRenderSize = INIT_RENDER_SIZE;

	inline static ImGuiIO* io;

	static void initialize();
	static void initDocking();

	static void update();
	static void updateDocking();

	static void drawScene();
	static void drawScene_displayInfo(bool barVisible);
	static void drawInspector();

	static void finalizeViewports();
};

constexpr glm::ivec2 ImGUIHandler::calculateInitialFullWindowSize()
{
	return {INIT_RENDER_SIZE.x + INSPECTOR_WIDTH, INIT_RENDER_SIZE.y};
}
