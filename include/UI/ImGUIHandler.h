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

	inline static bool _isInit = true;
	inline static bool _isAfterInit = true;

	inline static ImGuiIO* _io;

	static void init();
	static void initDocking();

	static void draw();
	static void updateDocking();

	static void finalizeViewports();

public:
	static constexpr glm::ivec2 INIT_RENDER_SIZE = {640 * 2, 360 * 2};
	static constexpr glm::ivec2 INIT_FULL_WINDOW_SIZE = {INIT_RENDER_SIZE.x, INIT_RENDER_SIZE.y};

	static bool isWindowFocused(WindowType type);
	static glm::vec2 getRelativeMousePos(WindowType type);

	friend class Program;
	friend class ImGUIWindowDrawer;
	friend class SDLHandler;
};

const char* windowTypeToString(WindowType type);
