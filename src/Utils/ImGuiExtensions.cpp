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

bool ImGui::LabeledText(const char* label, const char* text, int flags)
{
	return LabeledInput(label, Text, flags, text);
}
bool ImGui::LabeledFloat(const char* label, float& value, int flags, const char* format)
{
	return LabeledInput(label, InputFloat, flags, &value, 0.0f, 0.0f, format);
}
bool ImGui::LabeledInt(const char* label, int& value, int flags)
{
	float val = value;
	bool result = LabeledFloat(label, val, flags, "%.0f");
	value = (int)val;
	return result;
}
bool ImGui::LabeledInt(const char* label, int value, int flags)
{
	float val = value;
	return LabeledFloat(label, val, flags, "%.0f");
}
bool ImGui::LabeledInputFloat2(const char* label, float* values, int flags, const char* format)
{
	return LabeledInput(label, InputFloat2, flags, values, format);
}
bool ImGui::LabeledInputFloat3(const char* label, float* values, int flags, const char* format)
{
	return LabeledInput(label, InputFloat3, flags, values, format);
}
bool ImGui::LabeledInputFloat4(const char* label, float* values, int flags, const char* format)
{
	return LabeledInput(label, InputFloat4, flags, values, format);
}
bool ImGui::LabeledColorEdit3(const char* label, float* color, int flags)
{
	return LabeledInput(label, ColorEdit3, flags, color);
}
bool ImGui::LabeledColorEdit4(const char* label, float* color, int flags)
{
	return LabeledInput(label, ColorEdit4, flags, color);
}
bool ImGui::LabeledSliderInt(const char* label, int& value, int min, int max, int flags, const char* format)
{
	return LabeledInput(label, SliderInt, flags, &value, min, max, format);
}
bool ImGui::LabeledSliderFloat(const char* label, float& value, float min, float max, int flags, const char* format)
{
	return LabeledInput(label, SliderFloat, flags, &value, min, max, format);
}
bool ImGui::LabeledCheckbox(const char* label, bool& value, int flags)
{
	return LabeledInputNoFlags(label, Checkbox, flags, &value);
}
