#include"widget.h"
#include"menu.h"



using namespace ImGui;







bool ImGuiW::Button(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags,bool select,int& alpha, int& anim_size,int select_mark_size)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	float t = hovered ? 1.0f : 0.0f;
	if (g.HoveredId == g.CurrentWindow->GetID(label)){
		float t_anim = ImSaturate((g.HoveredIdNotActiveTimer) / 0.10f);
		t = hovered ? (t_anim) : (1.f - t_anim);
	}

	float t_active = select;
	if (g.LastActiveId == g.CurrentWindow->GetID(label))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / 0.10f);
		t_active =  t_anim;
	}
	// Render
	const ImU32 col = (held && hovered) ? GetColorU32(ImGuiCol_ButtonActive) : GetColorU32(ImLerp(GetStyleColorVec4(ImGuiCol_Button), GetStyleColorVec4(ImGuiCol_ButtonHovered), t));
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	//绘制选择条
	//使用外部数据做动画是不错的选择，尤其是这玩意不会被频繁地使用的时候
	if (select || alpha) {

		if (!select)
			alpha-=5;
		else if (alpha < 255)
			alpha+=5;

		ImClamp(alpha, 0, 255);

		auto v1 = g_MenuManager->color;
		v1.Value.w = alpha / 255.f;

		RenderFrame(bb.Min, ImVec2(bb.Min.x + select_mark_size, bb.Max.y), v1);
	}
	
	

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");
	
	if (!select)
	{
		if (anim_size)
			anim_size--;
	}
		
	else if (select && anim_size < select_mark_size)
		anim_size++;
	ImClamp(anim_size,0,5);


	ImGui::PushStyleColor(ImGuiCol_Text, ImLerp(ImGui::GetStyleColorVec4(ImGuiCol_Text), g_MenuManager->color, t_active));
	RenderTextClipped(bb.Min + style.FramePadding + ImVec2(anim_size,0), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	ImGui::PopStyleColor();

	// Automatically close popups
	//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
	//    CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

