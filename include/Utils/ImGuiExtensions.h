#pragma once

#include "imgui_internal.h"
#include "ImGUIHandler.h"

namespace ImGui
{
	inline ImGuiWindow* FindWindowByType(WindowType type)
	{
		return FindWindowByName(windowTypeToString(type));
	}
}
