#include "hooks.h"

#include "../../external/safetyhook/safetyhook.hpp"
#include "../utilities/memory.h"
#include "../interfaces/interfaces.h"
#include "../menu/menu.h"

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"
namespace g_hooks {

	namespace DX11 {
		safetyhook::InlineHook hook_present;
		safetyhook::InlineHook hook_resizebuffers;
		safetyhook::InlineHook hook_createswapchain;
		static WNDPROC o_WndProc;
		static std::once_flag g_InputInit;
	}
}


static LRESULT g_hooks::DX11::hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::call_once(g_hooks::DX11::g_InputInit, [hWnd]() {
		/*ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(g_interfaces->Device, g_interfaces->DeviceContext);

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = io.LogFilename = nullptr;*/

		g_MenuManager->init(hWnd, g_interfaces->Device, g_interfaces->DeviceContext);

		});

	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (g_MenuManager->show_menu) {
		// Messages handled by 'ImGui_ImplWin32_WndProcHandler'.
		switch (uMsg) {
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_NCMOUSELEAVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_CHAR:
		case WM_SETCURSOR:
		case WM_DEVICECHANGE:
			return 1;
		}
	}

	return CallWindowProc(g_hooks::DX11::o_WndProc, hWnd, uMsg, wParam, lParam);
}



//可能需要缺乏一些必要的检查
HRESULT __stdcall g_hooks::DX11::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{

	g_MenuManager->frame();

    return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
}


//设备 设备上下文等会在启动时被接口系统初始化
//如果更改了窗口分辨率CreateSwapChain会先被调用
//清除，随后在ResizeBuffers里程序初始化
//绘制函数只需要进行一些检查，免除任何维护
//可能...需要重新初始化imgui?
HRESULT __fastcall g_hooks::DX11::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	auto rt = hook_resizebuffers.call<HRESULT>(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);

	//重新创建各种东西
	if(rt == S_OK)
		g_MenuManager->on_resizebuffers();
 
	return rt;
}

HRESULT __stdcall g_hooks::DX11::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	if (g_interfaces->RenderTargetView != nullptr)
		g_interfaces->RenderTargetView->Release();
	g_interfaces->RenderTargetView = nullptr;

	return hook_createswapchain.call<HRESULT>(pFactory, pDevice, pDesc, ppSwapChain);
}


#define HK(N,S,P,I,F)	S = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(P,I)), reinterpret_cast<void*>(F));		\
					LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get("hook_log"),N);

bool g_hooks::init()
{
	IDXGIDevice* pDXGIDevice = NULL;
	g_interfaces-> Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter = NULL;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory* pIDXGIFactory = NULL;
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

	//挂钩窗口过程
	auto hwnd = FindWindowW(L"SDL_app", nullptr);
	auto hwnd1 = FindWindowW(nullptr, L"Counter-Strike 2");
	g_hooks::DX11::o_WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hwnd1, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_hooks::DX11::hkWndProc)));
	HK("Present", DX11::hook_present, g_interfaces->SwapChainDx11->pDXGISwapChain, 8, DX11::Present);
	HK("ResizeBuffers", DX11::hook_resizebuffers, g_interfaces->SwapChainDx11->pDXGISwapChain, 13, DX11::ResizeBuffers);
	HK("CreateSwapChain", DX11::hook_createswapchain, pIDXGIFactory, 10, DX11::CreateSwapChain);



	pDXGIDevice->Release();
	pDXGIDevice = nullptr;
	pDXGIAdapter->Release();
	pDXGIAdapter = nullptr;
	pIDXGIFactory->Release();
	pIDXGIFactory = nullptr;

    //DX11::hook_present = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(g_interfaces->SwapChainDx11->pDXGISwapChain,8)), reinterpret_cast<void*>(DX11::Present));
	//DX11::hook_resizebuffers = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(g_interfaces->SwapChainDx11->pDXGISwapChain, 13)), reinterpret_cast<void*>(DX11::ResizeBuffers));

    return 1;
}


void g_hooks::unhook(){
	DX11::hook_present.reset();
	DX11::hook_resizebuffers.reset();
	DX11::hook_createswapchain.reset();

}