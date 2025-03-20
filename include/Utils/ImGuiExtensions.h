#pragma once

#include "imgui_internal.h"
#include "ImGuiHandler.h"

namespace ImGui
{
	inline ImGuiWindow* FindWindowByType(WindowType type)
	{
		return FindWindowByName(windowTypeToString(type));
	}
}
