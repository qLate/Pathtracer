#pragma once

#include "ImGuiHandler.h"

class WindowDrawer
{
	inline static bool _showInspector = true;
	inline static bool _showStats = true;
	inline static bool _showIcons = true;

	inline static glm::ivec2 _currRenderSize = ImGuiHandler::INIT_RENDER_SIZE;

	static void init();

	static void drawWindows();

	static void drawMenuBar();
	static void drawScene();
	static void displayStats(bool barVisible);
	static void drawInspector();

public:
	static bool showInspector() { return _showInspector; }
	static bool showStats() { return _showStats; }
	static bool showIcons() { return _showIcons; }
	static glm::ivec2 currRenderSize() { return _currRenderSize; }

	static void setShowInspector(bool show) { _showInspector = show; }
	static void setShowStats(bool show) { _showStats = show; }

	friend class ImGuiHandler;
};
