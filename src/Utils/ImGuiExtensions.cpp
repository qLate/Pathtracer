// ReSharper disable CppInconsistentNaming
#include "ImGuiExtensions.h"

#include <string>

ImVec2 operator-(const ImVec2& max, const ImVec2& rhs)
{
	return ImVec2 {max.x - rhs.x, max.y - rhs.y};
}

ImGuiWindow* ImGui::FindWindowByType(WindowType type)
{
	return FindWindowByName(windowTypeToString(type));
}

bool ImGui::operator&(ItemLabelFlag lhs, ItemLabelFlag rhs)
{
	return (bool)((int)lhs & (int)rhs);
}

void ImGui::ItemLabel(std::string_view title, ItemLabelFlag flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	const ImVec2 lineStart = GetCursorScreenPos();
	const ImGuiStyle& style = GetStyle();
	float fullWidth = GetContentRegionAvail().x;
	float itemWidth = CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 textSize = CalcTextSize(title.data(), title.data() + title.length());
	ImRect textRect;
	textRect.Min = GetCursorScreenPos();
	if (flags & ItemLabelFlag::Right)
		textRect.Min.x = textRect.Min.x + itemWidth;
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	SetCursorScreenPos(textRect.Min);

	AlignTextToFramePadding();
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset;

	ItemSize(textRect);
	if (ItemAdd(textRect, window->GetID(title.data(), title.data() + title.size())))
	{
		RenderTextEllipsis(GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x, textRect.Max.x, title.data(), title.data() + title.size(), &textSize);

		if (textRect.GetWidth() < textSize.x && IsItemHovered())
			SetTooltip("%.*s", (int)title.size(), title.data());
	}
	if (flags & ItemLabelFlag::Left)
	{
		SetCursorScreenPos(textRect.Max - ImVec2 {0, textSize.y + window->DC.CurrLineTextBaseOffset});
		SameLine();
	}
	else if (flags & ItemLabelFlag::Right)
		SetCursorScreenPos(lineStart);
}

bool ImGui::LabeledText(const char* label, const char* text, ImGuiInputTextFlags flags)
{
	return LabeledInput(label, Text, flags, text);
}
bool ImGui::LabeledValue(const char* label, float value, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat, flags, &value, 0.0f, 0.0f, format);
}
bool ImGui::LabeledInt(const char* label, int value, ImGuiInputTextFlags flags)
{
	return LabeledValue(label, (float)value, flags, "%.0f");
}
bool ImGui::LabeledInputFloat2(const char* label, float* values, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat2, flags, values, format);
}
bool ImGui::LabeledInputFloat3(const char* label, float* values, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat3, flags, values, format);
}
bool ImGui::LabeledInputFloat4(const char* label, float* values, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat4, flags, values, format);
}
bool ImGui::LabeledColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags)
{
	return LabeledInput(label, ColorEdit4, flags, color);
}

bool ImGui::LabeledValue(const char* label, float value, bool& isDirty, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat, flags, isDirty, &value, 0.0f, 0.0f, format);
}
bool ImGui::LabeledInt(const char* label, int value, bool& isDirty, ImGuiInputTextFlags flags)
{
	return LabeledValue(label, (float)value, isDirty, flags, "%.0f");
}
bool ImGui::LabeledInputFloat2(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat2, flags, isDirty, values, format);
}
bool ImGui::LabeledInputFloat3(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat3, flags, isDirty, values, format);
}
bool ImGui::LabeledInputFloat4(const char* label, float* values, bool& isDirty, ImGuiInputTextFlags flags, const char* format)
{
	return LabeledInput(label, InputFloat4, flags, isDirty, values, format);
}
bool ImGui::LabeledColorEdit4(const char* label, float* color, bool& isDirty, ImGuiColorEditFlags flags)
{
	return LabeledInput(label, ColorEdit4, flags, isDirty, color);
}
