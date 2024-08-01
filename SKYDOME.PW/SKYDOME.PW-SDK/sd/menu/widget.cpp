#include"widget.h"
#include"menu.h"



using namespace ImGui;



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

struct slider_state {

	ImVec4 background_color, background_slider, circle_color, text_color;
	float slow, hint_slider;
};
///////////////////////////////////////////// TABS

inline ImVec4 tab_text_active = ImColor(255, 255, 255, 255);
inline ImVec4 tab_text_hov = ImColor(98, 112, 122, 255);
inline ImVec4 tab_text = ImColor(88, 92, 112, 255);

inline ImVec4 tab_active = ImColor(88, 109, 202, 255);
inline ImVec4 glow_active = ImColor(21, 226, 96, 230);

///////////////////////////////////////////// TEXT

inline ImVec4 text_active = ImColor(255, 255, 255, 255);
inline ImVec4 text_hov = ImColor(76, 75, 80, 255);
inline ImVec4 text = ImColor(66, 65, 70, 255);

///////////////////////////////////////////// CHILD

inline ImVec4 child_background = ImColor(22, 21, 26, 255);
inline ImVec4 border_child = ImColor(28, 27, 32, 255);
inline ImVec4 child_gradient = ImColor(218, 96, 21, 255);

inline ImVec4 border_child_default = ImColor(22, 21, 26, 255);
inline ImVec4 child_name = ImColor(62, 61, 65, 255);

///////////////////////////////////////////// GRADIENT

inline ImVec4 checkbox_gradient_start = ImColor(218, 96, 21, 255);
inline ImVec4 checkbox_gradient_end = ImColor(238, 96, 21, 255);

///////////////////////////////////////////// CHECKBOXES

inline ImVec4 checkbox_bg = ImColor(17, 16, 21, 255);
inline ImVec4 checkbox_mark = ImColor(255, 255, 255, 255);

///////////////////////////////////////////// SLIDERS

inline ImVec4 slider_background = ImColor(17, 16, 21, 255);
inline ImVec4 circle_push_gradient = ImColor(255, 255, 255, 255);
inline ImVec4 circle_pop_gradient = ImColor(255, 255, 255, 255);

///////////////////////////////////////////// INPUTTEXT

inline ImVec4 input_push_gradient = ImColor(27, 26, 31, 255);
inline ImVec4 input_pop_gradient = ImColor(17, 16, 21, 255);


///////////////////////////////////////////// SCROLLBAR

inline ImVec4 scroll_background = ImColor(15, 17, 20, 255);
inline ImVec4 scroll_keep = ImColor(36, 35, 40, 255);

///////////////////////////////////////////// COMBOBOX

inline ImVec4 combo_background = ImColor(28, 27, 32, 255);

inline ImVec4 combo_push_gradient = ImColor(27, 26, 31, 255);
inline ImVec4 combo_pop_gradient = ImColor(17, 16, 21, 255);

///////////////////////////////////////////// BUTTON

inline ImVec4 button_push_gradient_active = ImColor(27, 26, 31, 255);
inline ImVec4 button_pop_gradient_active = ImColor(17, 16, 21, 255);

inline ImVec4 button_push_gradient_hovered = ImColor(30, 29, 34, 255);
inline ImVec4 button_pop_gradient_hovered = ImColor(19, 20, 24, 255);

inline ImVec4 button_push_gradient = ImColor(27, 26, 31, 255);
inline ImVec4 button_pop_gradient = ImColor(17, 16, 21, 255);

///////////////////////////////////////////// KEYBIND

inline ImVec4 keybind_background = ImColor(18, 18, 20, 255);
inline ImVec4 keybind_border = ImColor(16, 16, 18, 255);

///////////////////////////////////////////// TAB VISIBLE

inline ImVec4 tab_push_gradient = ImColor(0, 145, 92, 255);
inline ImVec4 tab_pop_gradient = ImColor(21, 226, 96, 255);

bool ImGuiW::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	SetCursorPos(GetCursorPos() + ImVec2(0, 16));

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + 54, label_size.y + 5));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
		return false;

	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;

	const bool hovered = ItemHoverable(frame_bb, id,g.LastItemData.InFlags);
	bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
	if (!temp_input_is_active)
	{

		const bool input_requested_by_tabbing = false/*temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0*/;
		const bool clicked = hovered && IsMouseClicked(0, ImGuiInputFlags_None,id);
		const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id /*|| g.NavActivateInputId == id*/);
		if (make_active && clicked)
			SetKeyOwner(ImGuiKey_MouseLeft, id);
		if (make_active && temp_input_allowed)
			if ((clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
				temp_input_is_active = true;

		if (make_active && !temp_input_is_active)
		{
			SetActiveID(id, window);
			SetFocusID(id, window);
			FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}
	}

	if (temp_input_is_active);

	ImRect grab_bb;

	static std::map<ImGuiID, slider_state> anim;
	auto it_anim = anim.find(id);

	if (it_anim == anim.end())
	{
		anim.insert({ id, slider_state() });
		it_anim = anim.find(id);
	}

	//  it_anim->second.background_color = ImLerp(it_anim->second.background_color, IsItemActive() ? slider_active : hovered ? slider_hov : slider, g.IO.DeltaTime * 6.f);
	//  it_anim->second.background_slider = ImLerp(it_anim->second.background_slider, IsItemActive() ? bg_slider_active : hovered ? bg_slider_hov : bg_slider, g.IO.DeltaTime * 6.f);

	it_anim->second.text_color = ImLerp(it_anim->second.text_color, IsItemActive() ? text_active : hovered ? text_hov : text, g.IO.DeltaTime * 12.f);
	it_anim->second.hint_slider = ImLerp(it_anim->second.hint_slider, IsItemHovered() ? 1.f : 0.f, g.IO.DeltaTime * 35.f);

	window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0, 8 * g_MenuManager->GetScale()), frame_bb.Max - ImVec2(0, 3), GetColorU32(slider_background), 100);

	const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	if (value_changed) MarkItemEdited(id);

	if (grab_bb.Max.x > grab_bb.Min.x) {

		it_anim->second.slow = ImLerp(it_anim->second.slow, grab_bb.Min.x + 5 - frame_bb.Min.x, g.IO.DeltaTime * 20.f);

		const int vtx_idx_1 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(0, 8) * g_MenuManager->GetScale(), frame_bb.Min + ImVec2(it_anim->second.slow - 7 * g_MenuManager->GetScale(), 16.5f * g_MenuManager->GetScale()) , g_MenuManager->color, 100, ImDrawFlags_RoundCornersLeft);
		ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtx_idx_1, ImGui::GetWindowDrawList()->VtxBuffer.Size, frame_bb.Min, frame_bb.Max, g_MenuManager->color, g_MenuManager->color1);

		const int vtx_idx_2 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		window->DrawList->AddCircleFilled(frame_bb.Min + ImVec2(it_anim->second.slow - 1, 12.f * g_MenuManager->GetScale()), 8, GetColorU32(circle_push_gradient), 100);
		ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtx_idx_2, ImGui::GetWindowDrawList()->VtxBuffer.Size, frame_bb.Min, frame_bb.Min + ImVec2(7, 10), GetColorU32(circle_push_gradient), GetColorU32(circle_pop_gradient));
	}

	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

	PushStyleColor(ImGuiCol_Text, GetColorU32(it_anim->second.text_color));
	RenderTextClipped(frame_bb.Min - ImVec2(0, 20), frame_bb.Max - ImVec2(0, 20), value_buf, value_buf_end, NULL, ImVec2(1.0f, 0.5f));
	if (label_size.x > 0.0f) RenderText(ImVec2(frame_bb.Max.x - w - 54, frame_bb.Min.y - 20), label);
	PopStyleColor();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return value_changed;
}