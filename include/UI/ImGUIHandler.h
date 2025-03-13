#pragma once

#include "imgui.h"
#include "glm/vec2.hpp"

struct ImGuiWindow;

enum class WindowType
{
	Scene = 0,
	Inspector = 1,
};

class ImGUIHandler
{
	static constexpr char GLSL_VERSION[] = "#version 460";
	static constexpr int INSPECTOR_WIDTH = 300;

	inline static bool isInit = true;
	inline static bool isAfterInit = true;

public:
	static constexpr glm::ivec2 INIT_RENDER_SIZE = {640 * 2, 360 * 2};
	static constexpr glm::ivec2 INIT_FULL_WINDOW_SIZE = {INIT_RENDER_SIZE.x, INIT_RENDER_SIZE.y};

	inline static ImGuiIO* io;

	// --- Core ---
	static void init();
	static void initDocking();

	static void draw();
	static void updateDocking();

	static void finalizeViewports();
	// --- Core ---

	static bool isWindowFocused(WindowType type);

	friend class ImGUIWindowDrawer;
};

const char* windowTypeToString(WindowType type);
