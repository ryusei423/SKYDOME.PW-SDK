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

	}
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
	const auto isMainWindow = [handle]() {
		return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle) && handle != GetConsoleWindow();
		};

	DWORD pID = 0;
	GetWindowThreadProcessId(handle, &pID);

	if (GetCurrentProcessId() != pID || !isMainWindow()) {
		return TRUE;
	}

	*reinterpret_cast<HWND*>(lParam) = handle;
	return FALSE;
}

static LRESULT hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(g_hooks::DX11::o_WndProc, hWnd, uMsg, wParam, lParam);
}



//可能需要缺乏一些必要的检查
HRESULT __stdcall g_hooks::DX11::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{

	if (!ImGui::GetCurrentContext()) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
	};

	if (!ImGui::GetIO().BackendRendererUserData) {
		if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_interfaces->Device)))) {
			g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);


			g_hooks::DX11::o_WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hooks::DX11::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hkWndProc)));
			ImGui_ImplWin32_Init(g_hooks::DX11::hwnd);
			ImGui_ImplDX11_Init(g_interfaces->Device, g_interfaces->DeviceContext);

			//CRenderer::Get().Initialize();
		}
		else {
			return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
		}
	}

	if (!g_interfaces->RenderTargetView) {
		g_MenuManager->CreateRenderTarget(pSwapChain);
		//CreateRenderTarget();
	}
	else {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("SKYDOME");
		ImGui::Text("hello world");

		ImGui::End();

		auto m_BackgroundDrawList = ImGui::GetBackgroundDrawList();

		char framerate[128];
		snprintf(framerate, IM_ARRAYSIZE(framerate), "Welcome [%d]\nFPS: %d", 1337,
			static_cast<int>(ImGui::GetIO().Framerate));

		m_BackgroundDrawList->AddText({ 17, 9 }, IM_COL32(0, 0, 0, 255), framerate);
		m_BackgroundDrawList->AddText({ 16, 8 }, IM_COL32(27, 227, 200, 255), framerate);


		ImGui::Render();
		g_interfaces->DeviceContext->OMSetRenderTargets(1, &g_interfaces->RenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

    return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
}


HRESULT __fastcall g_hooks::DX11::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	if (g_interfaces->RenderTargetView != nullptr)
		g_interfaces->RenderTargetView->Release();
	g_interfaces->RenderTargetView = nullptr;
 
	return hook_resizebuffers.call<HRESULT>(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
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
	

	//初始化窗口句柄
	EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&DX11::hwnd));

	g_MenuManager->create(DX11::hwnd);
	//g_MenuManager->init(hwnd, g_interfaces->Device, g_interfaces->DeviceContext);

	IDXGIDevice* pDXGIDevice = NULL;
	g_interfaces->Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter = NULL;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory* pIDXGIFactory = NULL;
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

	HK("Present", DX11::hook_present, /*g_interfaces->SwapChainDx11->pDXGISwapChain*/ g_interfaces->SwapChain, 8, DX11::Present);
	HK("ResizeBuffers", DX11::hook_resizebuffers, /*g_interfaces->SwapChainDx11->pDXGISwapChain*/g_interfaces->SwapChain, 13, DX11::ResizeBuffers);
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