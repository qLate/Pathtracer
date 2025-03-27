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
	bool LabeledInput(const char* label, Func func, int flags, Args&&... args)
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
	bool LabeledInputNoFlags(const char* label, Func func, int flags, Args&&... args)
	{
		if (flags & ImGuiInputTextFlags_ReadOnly)
		{
			PushItemFlag(ImGuiItemFlags_Disabled, true);
			PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
		}

		ItemLabel(label);
		SameLine();

		bool result = false;
		if constexpr (std::is_same_v<decltype(func((std::string("##") + label).c_str(), std::forward<Args>(args)...)), bool>)
		{
			result = func((std::string("##") + label).c_str(), std::forward<Args>(args)...);
		}
		else
			func((std::string("##") + label).c_str(), std::forward<Args>(args)...);


		if (flags & ImGuiInputTextFlags_ReadOnly)
		{
			PopStyleVar();
			PopItemFlag();
		}

		return result;
	}

	bool LabeledText(const char* label, const char* text, int flags);
	bool LabeledFloat(const char* label, float& value, int flags = 0, const char* format = "%.1f");
	bool LabeledInt(const char* label, int& value, int flags = 0);
	bool LabeledInt(const char* label, int value, int flags = 0);
	bool LabeledInputFloat2(const char* label, float* values, int flags = 0, const char* format = "%.1f");
	bool LabeledInputFloat3(const char* label, float* values, int flags = 0, const char* format = "%.1f");
	bool LabeledInputFloat4(const char* label, float* values, int flags = 0, const char* format = "%.1f");
	bool LabeledColorEdit3(const char* label, float* color, int flags = 0);
	bool LabeledColorEdit4(const char* label, float* color, int flags = 0);
	bool LabeledSliderInt(const char* label, int& value, int min, int max, int flags = 0, const char* format = "%d");
	bool LabeledSliderFloat(const char* label, float& value, float min, float max, int flags = 0, const char* format = "%.3f");
	bool LabeledCheckbox(const char* label, bool& value, int flags = 0);
}
