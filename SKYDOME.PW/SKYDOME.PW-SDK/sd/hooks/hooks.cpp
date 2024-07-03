#include "hooks.h"

#include "../../external/safetyhook/safetyhook.hpp"
#include "../utilities/memory.h"
#include "../interfaces/interfaces.h"

safetyhook::InlineHook hook_present;

HRESULT __stdcall g_hooks::DX11::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{
    return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
}

bool g_hooks::init()
{
    hook_present = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(g_interfaces->SwapChainDx11->pDXGISwapChain,8)), reinterpret_cast<void*>(DX11::Present));

    return 1;
}
