// ReSharper disable CppInconsistentNaming
#pragma once

#include "imgui_internal.h"
#include "ImGuiHandler.h"
#include <string_view>
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

	template <typename T, typename Func, typename... Args,
	          std::enable_if_t<std::is_same_v<
		                           std::invoke_result_t<Func, const char*, T*, Args...>, bool>, int>  = 0>
	bool LabeledInput(const char* label, T& value, Func func, Args&&... args)
	{
		ItemLabel(label);
		SameLine();
		return func((std::string("##") + label).c_str(), &value, std::forward<Args>(args)...);
	}
	template <typename T, typename Func, typename... Args,
	          std::enable_if_t<std::is_same_v<
		                           std::invoke_result_t<Func, const char*, T*, Args...>, bool>, int>  = 0>
	bool LabeledInput(const char* label, T value[], Func func, Args&&... args)
	{
		ItemLabel(label);
		SameLine();
		return func((std::string("##") + label).c_str(), value, std::forward<Args>(args)...);
	}
	template <typename T, typename Func, typename... Args,
	          std::enable_if_t<std::is_void_v<
		                           std::invoke_result_t<Func, const char*, T*, Args...>>, int>  = 0>
	bool LabeledInput(const char* label, T& value, Func func, Args&&... args)
	{
		ItemLabel(label);
		SameLine();
		func((std::string("##") + label).c_str(), &value, std::forward<Args>(args)...);
		return false;
	}

	bool LabeledText(const char* label, const char* text);
	bool LabeledValue(const char* label, float value, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat2(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat3(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat4(const char* label, float* values, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags = 0);

	// With dirty flag
	bool LabeledValue(const char* label, float value, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat2(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat3(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledInputFloat4(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags = 0, const char* format = "%.3f");
	bool LabeledColorEdit4(const char* label, float* color, bool& isDirty, ImGuiInputTextFlags flags = 0);
}
