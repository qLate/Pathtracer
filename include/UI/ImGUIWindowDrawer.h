#pragma once

#include "ImGUIHandler.h"

class ImGUIWindowDrawer
{
	inline static bool showInspector = true;
	inline static bool showStats = true;

	inline static glm::ivec2 currRenderSize = ImGUIHandler::INIT_RENDER_SIZE;

	static void drawMenuBar();

	static void drawScene();
	static void drawScene_displayStats(bool barVisible);
	static void drawInspector();

	friend class ImGUIHandler;
};
