// ReSharper disable CppInconsistentNaming
#pragma once

#include "imgui_internal.h"
#include "ImGuiHandler.h"
#include <string_view>
#include <imgui.h>

ImVec2 operator-(const ImVec2& max, const ImVec2& rhs);

namespace ImGui
{
	ImGuiWindow* FindWindowByType(WindowType type);

	enum class ItemLabelFlag
	{
		Left = 1u << 0u,
		Right = 1u << 1u,
		Default = Left,
	};
	bool operator&(ItemLabelFlag lhs, ItemLabelFlag rhs);

	void ItemLabel(std::string_view title, ItemLabelFlag flags = ItemLabelFlag::Left);

	bool LabeledFloat(const char* label, float* value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	bool LabeledInputFloat2(const char* label, float* values, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	bool LabeledInputFloat3(const char* label, float* values, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	bool LabeledInputFloat4(const char* label, float* values, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	bool LabeledColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags = 0);
	bool LabeledColorPicker4(const char* label, float* color, ImGuiColorEditFlags flags = 0);
}
