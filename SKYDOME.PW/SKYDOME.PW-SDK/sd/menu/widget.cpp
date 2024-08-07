#include"widget.h"
#include"menu.h"



using namespace ImGui;


bool ImGuiW::MakeCombo(const char* label, const char* const items[], bool* v, int cout) {

	std::string preview;
	for (int i = 0; i < cout; i++) {
		if (v[i]) {
			preview += items[i];
			preview += ',';

		}

	}
	if (!preview.empty() && preview.back() == ',') {
		preview.pop_back();
	}

	if (BeginCombo(label, preview.c_str(), 0)) {

		for (int i = 0; i < cout; i++) {
			ImGui::Selectable(items[i], &v[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
		}
		ImGui::EndCombo();
	}



	return false;
}


bool ImGuiW::Checkbox(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);//获取文字大小

	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(window->Size.x - style.FramePadding.x * 4, label_size.y + style.FramePadding.y * 2));
	ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;
	/*if (label_size.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);*/
	if (label_size.x > 0.0f)
		RenderText(check_bb.GetTL(), label);

	if (!ItemAdd(total_bb, id))
		return false;
	//window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(ImVec4(1, 1, 1, 1)), style.GrabRounding);
	const ImRect box_bb(check_bb.GetTR().x - style.FramePadding.x * 1 - label_size.y,
		check_bb.GetTR().y,
		check_bb.GetBR().x - style.FramePadding.x * 0,
		check_bb.GetBR().y - style.FramePadding.y);

	bool hovered, held;
	bool pressed = ButtonBehavior(box_bb, id, &hovered, &held);
	if (ImGui::IsItemClicked())
		*v = !(*v);


	float t = *v ? 1.0f : 0.0f;

	if (g.LastActiveId == g.CurrentWindow->GetID(label))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / 0.40f);
		t = *v ? (t_anim) : (1.f - t_anim);
	}

	RenderFrame(box_bb.Min, box_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
	//加了一点动画效果
	/*if (*v)
	{*/
	const float check_sz = ImMin(box_bb.GetWidth(), box_bb.GetHeight());
	const float pad = ImMax(1.0f, (float)(int)(check_sz / 6.0f));
	//window->DrawList->AddRectFilled(box_bb.Min + ImVec2(pad, pad), box_bb.Max - ImVec2(pad, pad), GetColorU32(ImLerp(ImColor(0, 0, 0, 0), GetStyleColorVec4(ImGuiCol_CheckMark), t)), style.FrameRounding);

	/*auto color1 = GetColorU32(ImVec4(49 / 255.f, 126 / 255.f, 232 / 255.f, ImLerp(0, 255 ,t) / 255.f));
	auto color2 = GetColorU32(ImVec4(34 / 255.f, 109 / 255.f, 221 / 255.f, ImLerp(0, 255, t) / 255.f));*/
	/*auto color1 = GetColorU32(ImVec4(132 / 255.f, 132 / 255.f, 132 / 255.f, ImLerp(0, 255, t) / 255.f));
	auto color2 = GetColorU32(ImVec4(111 / 255.f, 111 / 255.f, 111 / 255.f, ImLerp(0, 255, t) / 255.f));*/


	ImVec4 tmp = g_MenuManager->color1;
	tmp.w = ImLerp(0, 255, t) / 255.f;

	ImU32 color2 = ImColor(tmp);
	
	
	tmp = g_MenuManager->color;

	tmp.w = ImLerp(0, 255, t) / 255.f;

	ImU32 color1 = ImColor(tmp);

	window->DrawList->AddRectFilledMultiColor(box_bb.Min + ImVec2(pad, pad), box_bb.Max - ImVec2(pad, pad), color1, color1, color2, color2);
	//}




	return pressed;
}



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

