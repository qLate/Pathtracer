#pragma once

#include "ImGuiHandler.h"

class WindowDrawer
{
	inline static bool _showInspector = true;
	inline static bool _showStats = true;

	inline static glm::ivec2 _currRenderSize = ImGuiHandler::INIT_RENDER_SIZE;

	static void drawMenuBar();
	static void drawScene();
	static void drawScene_displayStats(bool barVisible);
	static void drawInspector();

public:
	static glm::ivec2 currRenderSize() { return _currRenderSize; }

	static void setShowInspector(bool show) { _showInspector = show; }
	static void setShowStats(bool show) { _showStats = show; }

	friend class ImGuiHandler;
};
