#pragma once

#include "imgui.h"

class ImGUIHandler
{
	static constexpr char GLSL_VERSION[] = "#version 460";

public:
	inline static ImGuiIO* io;

	static void init();
	static void tryInitInitialDocking();

	static void update();
	static void updateImGui_drawScene();
	static void updateImGui_drawInspector();
	static void updateImGui_finalizeViewports();
};
