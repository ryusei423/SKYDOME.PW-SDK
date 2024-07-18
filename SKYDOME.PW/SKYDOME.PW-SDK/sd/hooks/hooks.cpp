#include "hooks.h"

#include "../../external/safetyhook/safetyhook.hpp"
#include "../utilities/memory.h"
#include "../interfaces/interfaces.h"
#include "../menu/menu.h"
#include"../../CheatData.h"

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

	namespace MouseInputEnabled {
		bool __fastcall MouseInputEnabled(void* pThisptr);
		safetyhook::InlineHook hook_MouseInputEnabled;
	}

	namespace RelativeModeMouse {

		void* __fastcall RelativeModeMouse(void* _this, int a2);
		safetyhook::InlineHook hook_RelativeModeMouse;
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

	//在游戏中时，鼠标会被锁定在中心
	//if (uMsg == WM_MOUSEMOVE) {
	//	LOG(INFO) << LOWORD(lParam) << " - "<<HIWORD(lParam);
	//}


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
			g_MenuManager->init(0,0,0);
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


		if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
			g_MenuManager->toggle(!g_MenuManager->show_menu);
			//RelativeModeMouse::hook_RelativeModeMouse.call<__int64>(g_interfaces->InputSystem , g_MenuManager->show_menu ? false : g_CheatData->RelativeLastValue);
		}

		g_CheatData->matrix = (Matrix3x4_t*)((std::ptrdiff_t)MEM::GetModuleBaseHandle(CLIENT_DLL) + cs2_dumper::offsets::client_dll::dwViewMatrix);

		g_MenuManager->frame(pSwapChain);


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


bool __fastcall g_hooks::MouseInputEnabled::MouseInputEnabled(void* pThisptr){
	return g_MenuManager->show_menu ? false : hook_MouseInputEnabled.call<bool>(pThisptr);
}


enum InputDeviceActions {
	ACTION_CURSOR_INVISIBLE = 0x01,      // 光标不可见 (0b00000001)
	ACTION_MOUSE_CAPTURE = 0x02,         // 鼠标捕获 (0b00000010)
	ACTION_CURSOR_CLIP = 0x04,           // 光标限制 (0b00000100)
	ACTION_RELATIVE_MOUSE = 0x08,        // 相对模式鼠标 (0b00001000)
	ACTION_STANDARD_CURSORS = 0x10,      // 标准光标 (0b00010000)
	ACTION_IME_ALLOWED = 0x40            // 允许输入法 (0b01000000)
};




void* __fastcall g_hooks::RelativeModeMouse::RelativeModeMouse(void* a1, int a2)
{
	//开启ISS输出
	//LOG(INFO) << "a1 + 0x28 -> " << *((unsigned int*) a1 + 0x28);
	//*((bool*)((void*)a1) + 0x40) = true;

	auto rt = hook_RelativeModeMouse.call<void*>(a1, a2);

	/*LOG(INFO) << a1 << " - " << a2 << "  rt->"<<std::hex << rt;

	if (a2 & ACTION_RELATIVE_MOUSE) {
		LOG(INFO) << "SETINGMOUSEMODE - > " << a1 << " - " << a2 << "  rt->" << std::hex << rt;

	}*/

	return rt;



	//bool enabled = g_MenuManager->show_menu ? false : hook_RelativeModeMouse.call<void*>(a1, a2);
	//g_CheatData->RelativeLastValue = enabled;

	//return enabled;
}


#define HK(N,S,P,I,F)	S = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(P,I)), reinterpret_cast<void*>(F));		\
					LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get("hook_log"),N);

#define HK_SIG(N,S,A,F)	S = safetyhook::create_inline(A, reinterpret_cast<void*>(F));		\
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

	//hook_setmousemode = safetyhook::create_inline(g_OffsetManager->offsets[g_OffsetManager->OFFSET_RELATIVE_MODE_MOUSE], reinterpret_cast<void*>(setmousemode));

	HK("MouseInputEnabled", MouseInputEnabled::hook_MouseInputEnabled, g_interfaces->CSGOInput, 13, g_hooks::MouseInputEnabled::MouseInputEnabled);

	HK_SIG("RelativeModeMouse",RelativeModeMouse::hook_RelativeModeMouse, g_OffsetManager->offsets[g_OffsetManager->OFFSET_RELATIVE_MODE_MOUSE], reinterpret_cast<void*>(RelativeModeMouse::RelativeModeMouse));


    return 1;
}


void g_hooks::unhook(){
	DX11::hook_present.reset();
	DX11::hook_resizebuffers.reset();
	DX11::hook_createswapchain.reset();

}