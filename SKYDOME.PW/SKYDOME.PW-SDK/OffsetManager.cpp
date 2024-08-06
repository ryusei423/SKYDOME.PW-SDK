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

	offsets[OFFSET_GET_BONE] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 49 63 F0 BA ? ? ? ? 48 8B D9 E8 ? ? ? ? 48 8B 8B")));
	offsets[OFFSET_GET_MATRIX_FOR_VIEW] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("40 53 48 81 EC ? ? ? ? 49 8B C1")));
	
	// #STR: "C:\\buildworker\\csgo_rel_win64\\build\\src\\game\\client\, "C:\\buildworker\\csgo_rel_win64\\build\\src\\game\\client\"
	offsets[OFFSET_FRAME_STAGE_NOTIFY] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 5C 24 10 56 48 83 EC 30 8B 05")));

	// #STR: "CountFilesNeedTrustCheck", "CountFilesCompletedTrustCheck", "BSecureAllowed", "CountItemsToReport", "GetTotalFilesLoaded"
	offsets[OFFSET_GET_FUNCTION] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 4C 24 ? 48 83 EC ? 48 8B 44 24 ? 48 83 78 ? ? 74 ? 48 8B 44 24")));

	// #STR: "ntdll.dll", "NtOpenFile", "NtQueryInformationThread", "kernelbase.dll", "LoadLibraryExW", "kernel32.dll", "GetCurrentThreadStackLimits", "GetVersionExA", "gameoverlayrenderer64.dll"
	offsets[OFFSET_GET_FUNCTION1] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 4C 24 ? 48 83 EC ? 48 8B 44 24 ? 48 83 78 ? ? 74 ? E9")));
	//很有意思,我们是否能用一种更好的方法来隐藏我们的模块？
	// 48 89 4C 24 ? 48 81 EC ? ? ? ? 48 C7 44 24 - DllVerification
	// 48 89 4C 24 ? 48 83 EC ? 48 83 3D - DllVerification2
	// 44 88 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? B8 - CUserMessage_DllStatus

	//RVA
	//41 8B D6 E8 ? ? ? ? 4C 8B F8
	//E8 ?? ?? ?? ?? 48 8B F0 48 85 C0 0F 84 35 02 00 00
	offsets[OFFSET_GET_HITBOX_SET] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 48 89 ?? ?? ?? 57 48 ?? ?? ?? ?? ?? ?? 8B DA 48 8B F9 E8 ?? ?? ?? ?? 48 8B F0 48 85 C0 0F ?? ?? ?? ?? ?? 48 8D")));

	offsets[OFFSET_GET_HITBOX_PARENT] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("85 D2 78 17 3B 91 78")));

	offsets[OFFSET_GET_HITBOX_FLAGS] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("85 D2 78 16 3B 91")));

	offsets[OFFSET_GET_HITBOX_NAME] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("85 D2 78 25 3B 91")));

	offsets[OFFSET_GET_HITBOXS_NUM] = reinterpret_cast<void*>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL,
		XorStr("E8 ? ? ? ? 8B D8 48 C7 44 24 50 00 00 00 00")), 1, 0));

	//RVA
	//E8 ? ? ? ? 45 33 F6 4C 63 E0 ptr.Absolute(1, 0);
	offsets[OFFSET_HITBOX_TO_WORLD_TRANSFORMS] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 55 57 41 54 41 56 41 57 48 ?? ?? ?? 45")));


	//#STR: "invalid_bone", "default", "invalid_hitbox"
	//这个字符串的函数并不是我们要找的，但是我们要找的函数是唯一调用这个函数的
	offsets[OFFSET_TRACE_InitializeTraceInfo] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 57 48 ?? ?? ?? 48 8B D9 33 FF 48 ?? ?? ?? ?? ?? ?? 48"/*"48 89 5C 24 08 57 48 83 EC 20 48 8B D9 33 FF 48 8B 0D"*/)));

	offsets[OFFSET_TRACE_InitializeTrace] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 5C 24 08 57 48 83 EC 20 48 8B D9 33 FF 48 8B 0D")));
	//这俩是一个，但是我为其中一个重新生成了带有通配符的特征码
	//或许可以用的久一点?

	offsets[OFFSET_TRACE] = *reinterpret_cast<void**>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, 
		XorStr("4C 8B 3D ? ? ? ? 24 C9 0C 49 66 0F 7F 45 ?")), 0x3));

	offsets[OFFSET_TRACE_Init] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 48 89 ?? ?? ?? 57 48 ?? ?? ?? 0F B6 ?? ?? 33 F6"/*"48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B6 41 37 33"*/)));

	offsets[OFFSET_TRACE_ClipTraceToPlayers] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 8B C4 55 56 48 8D ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? 48 89 ?? ?? 49"/*"48 8B C4 55 56 48 8D A8 58"*/)));

	offsets[OFFSET_TRACE_get_trace_info] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 48 89 ?? ?? ?? 48 89 ?? ?? ?? 57 48 ?? ?? ?? 48 8B E9 0F 29"/*"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 60 48 8B E9 0F"*/)));

	offsets[OFFSET_TRACE_handle_bullet_penetration] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 8B C4 44 89 ?? ?? 55 57 41 55 41 57 48 8D"/*"48 8B C4 44 89 48 20 55 57 41 55"*/)));

	offsets[OFFSET_TRACE_CreateTrace] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 48 89 ?? ?? ?? 48 89 ?? ?? ?? 57 41 56 41 57 48 ?? ?? ?? F2"/*"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 40 F2"*/)));

	// #STR: "Physics/TraceShape (Client)"
	offsets[OFFSET_TRACE_TraceShape] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 ?? ?? ?? 48 89 ?? ?? ?? 48 89 ?? ?? ?? 48 89 ?? ?? ?? 55 41 54 41 55 41 56 41 57 48 8D ?? ?? ?? ?? ?? ?? B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 2B E0 65"/*"48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 48 89 4C 24 08 55 41 54 41 55 41 56 41 57 48 8D"*/)));


	// #STR: "allowed", "ISS: Cursor invisible from '%s'\n", "ISS: Cursor icon from '%s'\n", "ISS: Mouse capture enabled from '%s'\n", "ISS: Mouse capture disabled from '%s'\n", "ISS: Cursor clip %s from '%s'\n", "ISS: Relative mouse %s from '%s'\n", "ISS: Standard cursors from '%s'\n", "disabled", "enabled"
	offsets[OFFSET_RELATIVE_MODE_MOUSE] = reinterpret_cast<void*>(MEM::FindPattern(INPUTSYSTEM_DLL, 
		XorStr("40 55 41 57 48 ?? ?? ?? 8B 41 ?? 8B EA 4C 8B F9 85 C0 0F")));
	
	//CMaterialSystem2 -> 30
	offsets[OFFSET_CREATE_MATERIAL] = reinterpret_cast<void*>(MEM::FindPattern(MATERIAL_SYSTEM2_DLL,
		XorStr("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05")));
	
	offsets[OFFSET_CreateMaterialResource] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("40 53 48 83 EC 20 48 8B 01 48 8B D9 44")));

	offsets[OFFSET_GET_CLIENT_INTERP] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 ?? ?? ?? 0F 29 ?? ?? ?? 48 ?? ?? ?? ?? ?? ?? BA ?? ?? ?? ?? 0F 29 ?? ?? ?? E8 ?? ?? ?? ??")));

	// CCSGOINPUT -> 15 调试函数
	// #STR: "cl: CreateMove - Invalid player history [ %d, %d, %.3f ] f
	// 这个函数调用此函数来获得结果
	offsets[OFFSET_CalculateInterpInfos] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 48 8B F9 48 8D 44 24")));

	// #STR: "Bone merge bones from parent were invalid: parent model '%, "C:\\buildworker\\csgo_rel_win64\\build\\src\\game\\shared\"
	offsets[OFFSET_CalculateWorldSpaceBones] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 8B F1")));

	//RVA:E8 ? ? ? ? 45 84 FF 74 32
	//还没有弄清楚参数是什么，我的伪代码阅读能力有点糟糕，又或许是我没有耐心而已
	offsets[OFFSET_DRAW_HITBOX] = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL,
		XorStr("48 89 54 24 10 55 53 56 41 55 41 56 48")));

	offsets[OFFSET_GOBALVARS] = *reinterpret_cast<void**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL, 
		XorStr("48 89 0D ? ? ? ? 48 89 41")), 0x3, 0x7));

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
		LOG(FATAL) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_fail")), OFFSET_MAX, nullptr_cout);

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
