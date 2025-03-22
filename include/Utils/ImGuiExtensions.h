// ReSharper disable CppInconsistentNaming
#pragma once

#include <functional>

#include "imgui_internal.h"
#include "ImGuiHandler.h"
#include <imgui.h>
#include <string>

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

	template <typename Func, typename... Args>
	bool LabeledInput(const char* label, Func func, ImGuiInputTextFlags flags, Args&&... args)
	{
		if (flags & ImGuiInputTextFlags_ReadOnly)
		{
			PushItemFlag(ImGuiItemFlags_Disabled, true);
			PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
		}

		ItemLabel(label);
		SameLine();

		bool result = false;
		if constexpr (std::is_same_v<decltype(func((std::string("##") + label).c_str(), std::forward<Args>(args)..., flags)), bool>)
		{
			result = func((std::string("##") + label).c_str(), std::forward<Args>(args)..., flags);
		}
		else
			func((std::string("##") + label).c_str(), std::forward<Args>(args)..., flags);


		if (flags & ImGuiInputTextFlags_ReadOnly)
		{
			PopStyleVar();
			PopItemFlag();
		}

		return result;
	}
	template <typename Func, typename... Args>
	bool LabeledInput(const char* label, Func func, ImGuiInputTextFlags flags, bool& isDirty, Args&&... args)
	{
		if (LabeledInput(label, func, flags, std::forward<Args>(args)...))
		{
			isDirty = true;
			return true;
		}
		return false;
	}

	bool LabeledText(const char* label, const char* text, ImGuiInputTextFlags flags);
	bool LabeledValue(const char* label, float value, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInt(const char* label, int value, ImGuiInputTextFlags flags = 0);
	bool LabeledInputFloat2(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat3(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat4(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags = 0);

	// With dirty flag
	bool LabeledValue(const char* label, float value, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInt(const char* label, int value, bool& isDirty, ImGuiInputTextFlags flags = 0);
	bool LabeledInputFloat2(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat3(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat4(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledColorEdit4(const char* label, float* color, bool& isDirty, ImGuiInputTextFlags flags = 0);
}
