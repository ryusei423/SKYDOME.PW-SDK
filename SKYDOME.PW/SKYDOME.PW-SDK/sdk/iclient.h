#pragma once
#include "../sd/utilities/memory.h"

class IClient
{
public:
	float get_lerp_time() {
		return MEM::CallVFunc<float,171>(this);
	}

	//c_scene_debug_overlay* get_scene_debug_overlay()
	//{
	//	//xref: CDebugOverlayGameSystem lea rdx
	//	// https://imgur.com/864KioY
	//	// https://imgur.com/MYz5ScT
	//	using function_t = c_scene_debug_overlay * (__thiscall*)(i_client*);
	//	return (*reinterpret_cast<function_t**>(std::uintptr_t(this)))[155](this);
	//}
};