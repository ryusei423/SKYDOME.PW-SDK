#pragma once
#include <cstdint>

class CInputSystem {
   public:

    bool IsRelativeMouseMode() {
		// @ida: 'IInputSystem::SetRelativeMouseMode'.

		// Offset in 'CInputSystem::SetRelativeMouseMode'.
		// Function is right above 'CInputSystem::DebugSpew'.
		return *reinterpret_cast<bool**>(reinterpret_cast<std::uintptr_t>(this) + 0x4F);
	};

    void* GetSDLWindow() {
		// @ida: IInputSystem::DebugSpew -> #STR: "Current coordinate bias %s: %g,%g scale %g,%g\n"
		return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(this) + 0x26A8);
	};

};
