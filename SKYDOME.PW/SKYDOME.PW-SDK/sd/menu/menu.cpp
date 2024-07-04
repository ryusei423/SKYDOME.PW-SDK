#include "menu.h"
// used: [ext] imgui
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_freetype.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"

#include "../utilities/memory.h"

static void* __cdecl ImGuiAllocWrapper(const std::size_t nSize, [[maybe_unused]] void* pUserData = nullptr)
{
	return MEM::HeapAlloc(nSize);
}

static void __cdecl ImGuiFreeWrapper(void* pMemory, [[maybe_unused]] void* pUserData = nullptr) noexcept
{
	MEM::HeapFree(pMemory);
}

bool MenuManager::init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	ImGui::SetAllocatorFunctions(ImGuiAllocWrapper, ImGuiFreeWrapper);

	ImGui::CreateContext();

	// setup platform and renderer bindings
	if (!ImGui_ImplWin32_Init(hWnd))
		return false;

	if (!ImGui_ImplDX11_Init(pDevice, pContext))
		return false;






	return true;
}
