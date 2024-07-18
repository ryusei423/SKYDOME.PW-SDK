#include"OffsetManager.h"
#include"gobal.h"
#include "sd/utilities/memory.h"

#include <chrono>
bool OffsetManager::scan()
{
	auto start = std::chrono::system_clock::now();
	bool rt = true;
	//额外处理swapchain
	//https://www.unknowncheats.me/forum/4089361-post4621.html

	auto swapchain = MEM::FindPattern(RENDERSYSTEM_DLL, XorStr("66 0F 7F 05 ?? ?? ?? ?? 48 89 2D ?? ?? ?? ?? 66 0F 7F 0D"));

	if (!swapchain)
	{
		LOG(ERROR) << g_CheatLocalization->get(XorStr("interfaces_swapchain_not_found"));
		return false;
	}

	do
	{
		swapchain++;
	} while (*(uint16_t*)swapchain != 0x0F66);


	offsets[OFFSET_SWAPCHAINDX11] = **reinterpret_cast<void***>(MEM::ResolveRelativeAddress(swapchain, 0x4, 0x8));


	

	offsets[OFFSET_CCSGOINPUT] = *reinterpret_cast</*CCSGOInput*/void**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL, 
		XorStr("48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 8B DF")), 0x3, 0x7));
	//@ida: #STR: "(missing),", "(missing)", "Ent %3d: %s class %s name %s\n" | or find "cl_showents" cvar -> look for callback
	offsets[OFFSET_GET_ENTITY_BY_INDEX] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL, 
		XorStr("81 FA ? ? ? ? 77 ? 8B C2 C1 F8 ? 83 F8 ? 77 ? 48 98 48 8B 4C C1 ? 48 85 C9 74 ? 8B C2 25 ? ? ? ? 48 6B C0 ? 48 03 C8 74 ? 8B 41 ? 25 ? ? ? ? 3B C2 75 ? 48 8B 01")));



	// #STR: "allowed", "ISS: Cursor invisible from '%s'\n", "ISS: Cursor icon from '%s'\n", "ISS: Mouse capture enabled from '%s'\n", "ISS: Mouse capture disabled from '%s'\n", "ISS: Cursor clip %s from '%s'\n", "ISS: Relative mouse %s from '%s'\n", "ISS: Standard cursors from '%s'\n", "disabled", "enabled"
	offsets[OFFSET_RELATIVE_MODE_MOUSE] = reinterpret_cast<void*>(MEM::FindPattern(INPUTSYSTEM_DLL, 
		XorStr("40 55 41 57 48 ?? ?? ?? 8B 41 ?? 8B EA 4C 8B F9 85 C0 0F")));
	
	


	int nullptr_cout = 0;

	for (int i = 0; i < OFFSET_MAX; i++) {
		if (!offsets[i]) {
			LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_check")), i, offsets[i]);
			nullptr_cout++;
			rt = false;
		}
		else{
			LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_check")), i, offsets[i]);
		}

	}

	if (rt){
		// 获取结束时间
		auto end = std::chrono::system_clock::now();
		// 计算时间差，单位为秒
		std::chrono::duration<double> elapsed = end - start;
		// 输出结果
		LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_success")), OFFSET_MAX, elapsed.count());
	}
	else {

		// 获取结束时间
		auto end = std::chrono::system_clock::now();
		// 计算时间差，单位为秒
		std::chrono::duration<double> elapsed = end - start;
		// 输出结果
		LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_fail")), OFFSET_MAX, nullptr_cout);

	}

	



	return rt;


}

bool OffsetManager::scan_export(){
	const void* hSDL3 = MEM::GetModuleBaseHandle(SDL3_DLL);

	fnSetRelativeMouseMode = reinterpret_cast<decltype(fnSetRelativeMouseMode)>(MEM::GetExportAddress(hSDL3, XorStr("SDL_SetRelativeMouseMode")));
	fnGetRelativeMouseMode = reinterpret_cast<decltype(fnGetRelativeMouseMode)>(MEM::GetExportAddress(hSDL3, XorStr("SDL_GetRelativeMouseMode")));
	fnSetWindowMouseGrab = reinterpret_cast<decltype(fnSetWindowMouseGrab)>(MEM::GetExportAddress(hSDL3, XorStr("SDL_SetWindowMouseGrab")));
	fnWarpMouseInWindow = reinterpret_cast<decltype(fnWarpMouseInWindow)>(MEM::GetExportAddress(hSDL3, XorStr("SDL_WarpMouseInWindow")));
	fnShowCursor = reinterpret_cast<decltype(fnShowCursor)>(MEM::GetExportAddress(hSDL3, XorStr("SDL_ShowCursor")));

	if (fnSetRelativeMouseMode && fnGetRelativeMouseMode && fnSetWindowMouseGrab && fnWarpMouseInWindow){
		LOG(INFO) << g_CheatLocalization->get("offset_manager_export_scan_success");
		return true;
	}
	LOG(INFO) << g_CheatLocalization->get("offset_manager_export_scan_fail");
	return false;
}
