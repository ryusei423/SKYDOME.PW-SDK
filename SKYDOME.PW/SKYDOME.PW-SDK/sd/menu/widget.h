#pragma once

#include <utility>
#include<Windows.h>
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"


#include "../../external/imgui/imgui_internal.h"
//#include "../../external/imgui/imgui_stdlib.h"


#pragma once
// used: std::function
#include <functional>
// used: std::deque
#include <deque>
// used: std::unique_lock
#include <mutex>
// used: std::shared_mutex
#include <shared_mutex>
// used: std::any
#include <any>

#include<array>
//#include "../sdk/valve_misc/Color.hpp"

namespace ImGuiW
{



	bool Button(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags, bool select, int& alpha, int& anim_size,int select_mark_size = 5);

	
}