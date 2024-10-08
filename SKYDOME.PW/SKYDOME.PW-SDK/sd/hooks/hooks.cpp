#include "hooks.h"

#include "../../external/safetyhook/safetyhook.hpp"
#include "../utilities/memory.h"
#include "../interfaces/interfaces.h"

#include"../../CheatData.h"

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_impl_dx11.h"

#include "../menu/menu.h"
#include "../esp/esp.h"
#include "../entitycache/entitycache.h"
#include "../ragebot/ragebot.h"
#include "../ragebot/playerlog.h"
#include "../ragebot/antiaim.h"
#include "../menu/config.h"
#include "../movement/movement.h"


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
	
	namespace GetMatrixForView {
		ViewMatrix_t* __fastcall GetMatrixForView(/*CRenderGameSystem**/void* pRenderGameSystem, /*IViewRender**/void* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels);
		safetyhook::InlineHook hook_GetMatrixForView;
	}

	namespace FrameStageNotify {
		void __fastcall FrameStageNotify(void* rcx, int nFrameStage);
		safetyhook::InlineHook hook_fsn;
	}

	namespace OnAddEntity {
		void* __fastcall OnAddEntity(void* rcx, CEntityInstance* pInstance, CBaseHandle hHandle);
		safetyhook::InlineHook hook_OnAddEntity;
	}

	namespace OnRemoveEntity {
		void* __fastcall OnRemoveEntity(void* rcx, CEntityInstance* pInstance, CBaseHandle hHandle);
		safetyhook::InlineHook hook_OnRemoveEntity;
	}

	namespace CreateMove {

		bool __fastcall CreateMove(CCSGOInput* pInput, int nSlot, bool nUnk, std::byte nUnk2);
		safetyhook::InlineHook hook_CreateMove;
	}

	namespace ValidateInput {
		void __fastcall ValidateInput(CCSGOInput* pInput, int unk);
		safetyhook::InlineHook hook_ValidateInput;
	}

	namespace TestHook {

		__int64 __fastcall SetRelative(__int64 a1, bool a2);
		safetyhook::InlineHook hook_SetRelative;

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

	//在菜单打开时可以移动，但是不能开火
	//菜单打开时啥都不能干的挂，coder要么不玩游戏，要么是弱智，还有一种可能是只有一只手的残疾人
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP){
		if (g_MenuManager->show_menu)
			return true;
	}


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
		
		if (*g_ConfigManager->GetBool(XorStr("esp_enable"))){
			g_EspDrawManager->MakeFrame();
			g_EspDrawManager->DrawFrame(ImGui::GetBackgroundDrawList());
		}
		
		if (!g_interfaces->EngineClient->IsInGame()||
			!g_interfaces->EngineClient->IsConnected()){
			if (g_interfaces->GlobalVars){
				g_interfaces->GlobalVars = nullptr;
			}
			
		}

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
	*((bool*)((void*)a1) + 0x40) = true;

	auto rt = hook_RelativeModeMouse.call<void*>(a1, a2);
	
	if (g_interfaces->EngineClient->IsConnected() && g_interfaces->EngineClient->IsInGame()){
		std::cout << rt << "\n";
		LOG(INFO) << a1 << " - " << a2 << "  rt->" << std::hex << rt;

		if ((ptrdiff_t)rt == 0xFFFFFFFF){
			g_CheatData->IsRelative = false;
		}

		if ((ptrdiff_t)rt != 0xFFFFFFFF && (rt == NULL || (ptrdiff_t)rt > 0x1000000)){
			g_CheatData->IsRelative = true;
		}
		else
		{
			g_CheatData->IsRelative = false;
		}
	}

	



	/*LOG(INFO) << a1 << " - " << a2 << "  rt->"<<std::hex << rt;

	if (a2 & ACTION_RELATIVE_MOUSE) {
		LOG(INFO) << "SETINGMOUSEMODE - > " << a1 << " - " << a2 << "  rt->" << std::hex << rt;

	}*/

	return rt;



	//bool enabled = g_MenuManager->show_menu ? false : hook_RelativeModeMouse.call<void*>(a1, a2);
	//g_CheatData->RelativeLastValue = enabled;

	//return enabled;
}

ViewMatrix_t* __fastcall g_hooks::GetMatrixForView::GetMatrixForView(void* pRenderGameSystem, void* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{
	auto rt = hook_GetMatrixForView.call<ViewMatrix_t*>(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

	g_EspDrawManager->matrix = *pOutWorldToProjection;

	//无用 只是感觉很好玩
	//pOutWorldToProjection = &g_EspDrawManager->matrix;

	return rt;
}


void* __fastcall g_hooks::OnAddEntity::OnAddEntity(void* rcx, CEntityInstance* pInstance, CBaseHandle hHandle)
{
	g_EntityCache->OnAdd(pInstance,hHandle);

	return hook_OnAddEntity.call<void*>(rcx, pInstance, hHandle);
}


void* __fastcall g_hooks::OnRemoveEntity::OnRemoveEntity(void* rcx, CEntityInstance* pInstance, CBaseHandle hHandle)
{
	g_EntityCache->OnRemove(pInstance, hHandle);
	g_PlayerLog->logs[hHandle.GetEntryIndex()].valid = false;
	return hook_OnRemoveEntity.call<void*>(rcx,pInstance,hHandle);
}




void __fastcall g_hooks::FrameStageNotify::FrameStageNotify(void* rcx, int nFrameStage){
	hook_fsn.call<void>(rcx,nFrameStage);
	if (nFrameStage == FRAME_NET_UPDATE_END){
		//更新看起来有些慢
		//可能是未经插值的位置？
		//g_EspDrawManager->MakeFrame();
		if (g_interfaces->EngineClient->IsConnected()&&
			g_interfaces->EngineClient->IsInGame()&&
			g_CheatData->LocalController &&
			g_CheatData->LocalController->IsPawnAlive()&&
			g_CheatData->LocalPawn){
			//g_CheatData->net_update_end_eyepos = g_CheatData->LocalPawn->GetEyePosition() + (g_CheatData->LocalPawn->m_vecAbsVelocity() * 0.03);
			g_PlayerLog->Log();
		}

		
		

	}
	
}

bool __fastcall g_hooks::CreateMove::CreateMove(CCSGOInput* pInput, int nSlot, bool nUnk, std::byte nUnk2){

	auto rt = hook_CreateMove.call<bool>(pInput, nSlot, nUnk, nUnk2);
	if (!g_interfaces->EngineClient->IsInGame() && !g_interfaces->EngineClient->IsConnected()) {
		g_interfaces->GlobalVars = nullptr;
		return rt;
	}


	//getlocalplayer注释里说 return CBaseHandle index
	//嗯...先试试能不能用吧
	//好吧它指的是CBaseHandle中的nIndex，而不是CBaseHandle的索引
	//还是不太熟悉CS2
	g_CheatData->LocalController = g_interfaces->GameResourceService->pGameEntitySystem->Get<CCSPlayerController>(g_interfaces->EngineClient->GetLocalPlayer());
	if(g_CheatData->LocalController)g_CheatData->LocalPawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(g_CheatData->LocalController->GetPawnHandle());

	if (!g_CheatData->LocalController || !g_CheatData->LocalController->IsPawnAlive() || !g_CheatData->LocalPawn){
		return rt;
	}

	g_CheatData->save_eyepos = g_CheatData->LocalPawn->GetEyePosition() + (g_CheatData->LocalPawn->m_vecAbsVelocity() * -0.01);
	//用于早期自动停止
	g_CheatData->pred_eyepos = g_CheatData->save_eyepos + (g_CheatData->LocalPawn->m_vecAbsVelocity() * 0.04);

	

	if (!g_interfaces->GlobalVars){
		g_interfaces->GlobalVars = *reinterpret_cast<IGlobalVars**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL,
			XorStr("48 89 0D ? ? ? ? 48 89 41")), 0x3, 0x7));;
	}


	CUserCmd* cmd = pInput->GetUserCmd();

	if (!cmd || 
		!cmd->csgoUserCmd.pBaseCmd || 
		!cmd->csgoUserCmd.pBaseCmd->pViewAngles){
		return rt;
	}

	

	g_MovementManager->InitTick(cmd);

	g_AntiAim->run(cmd);

	if (*g_ConfigManager->GetBool("ragebot_enable"))
		g_RageBot->run(cmd);

	if (cmd->csgoUserCmd.pBaseCmd->pInButtonState->nValue & IN_ATTACK){
		if(*g_ConfigManager->GetBool("ragebot_debug_cmd_info"))g_RageBot->DumpCmdInfo(cmd);
	}


	g_MovementManager->EndTick(cmd);


	return rt;
}

void __fastcall g_hooks::ValidateInput::ValidateInput(CCSGOInput* pInput, int unk){
	auto view = pInput->GetViewAngles();
	hook_ValidateInput.call<void>(pInput,unk);
	pInput->SetViewAngle(view);

}


__int64 __fastcall g_hooks::TestHook::SetRelative(__int64 a1, bool a2){

	//LOG(DEBUG) << a2;

	g_CheatData->IsRelative = a2;
	return hook_SetRelative.call<__int64>(a1,a2);
}



#define HK(N,S,P,I,F)	S = safetyhook::create_inline(reinterpret_cast<void*>(MEM::GetVFunc(P,I)), reinterpret_cast<void*>(F));		\
					LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get("hook_log"),N);

#define HK_SIG(N,S,A,F)	S = safetyhook::create_inline(A, reinterpret_cast<void*>(F));		\
					LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get("hook_log"),N);
bool g_hooks::init()
{
	

	//初始化窗口句柄
	EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&DX11::hwnd));

	//g_MenuManager->create(DX11::hwnd);
	//g_MenuManager->init(hwnd, g_interfaces->Device, g_interfaces->DeviceContext);

	g_interfaces->SwapChain = g_interfaces->GameSwapChain->pDXGISwapChain;
	g_interfaces->SwapChain->GetDevice(IID_PPV_ARGS(&g_interfaces->Device));
	g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);

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

	HK("MouseInputEnabled", MouseInputEnabled::hook_MouseInputEnabled, g_interfaces->CSGOInput, 16, g_hooks::MouseInputEnabled::MouseInputEnabled);
	HK("CreateMove", CreateMove::hook_CreateMove, g_interfaces->CSGOInput, 5, g_hooks::CreateMove::CreateMove);
	HK("ValidateInput", ValidateInput::hook_ValidateInput, g_interfaces->CSGOInput, 7, g_hooks::ValidateInput::ValidateInput);

	//sig:40 53 48 ?? ?? ?? 0F B6 DA
	//被 OFFSET_RELATIVE_MODE_MOUSE 调用
	//OFFSET_RELATIVE_MODE_MOUSE 实际是一个用于调整鼠标的综合函数，它调用此函数来设置相对模式
	//已经不再需要那个钩子
	HK("SetRelative", TestHook::hook_SetRelative, g_interfaces->InputSystem, 78, g_hooks::TestHook::SetRelative);


	HK("OnAddEntity", OnAddEntity::hook_OnAddEntity, g_interfaces->GameResourceService->pGameEntitySystem, 14, g_hooks::OnAddEntity::OnAddEntity);
	HK("OnRemoveEntity", OnRemoveEntity::hook_OnRemoveEntity, g_interfaces->GameResourceService->pGameEntitySystem, 15, g_hooks::OnRemoveEntity::OnRemoveEntity);

	//HK_SIG("RelativeModeMouse",RelativeModeMouse::hook_RelativeModeMouse, g_OffsetManager->offsets[g_OffsetManager->OFFSET_RELATIVE_MODE_MOUSE], reinterpret_cast<void*>(RelativeModeMouse::RelativeModeMouse));
	HK_SIG("GetMatrixForView", GetMatrixForView::hook_GetMatrixForView, g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_MATRIX_FOR_VIEW], reinterpret_cast<void*>(GetMatrixForView::GetMatrixForView));
	HK_SIG("FrameStageNotify", FrameStageNotify::hook_fsn, g_OffsetManager->offsets[g_OffsetManager->OFFSET_FRAME_STAGE_NOTIFY], reinterpret_cast<void*>(FrameStageNotify::FrameStageNotify));


    return 1;
}


void g_hooks::unhook(){
	DX11::hook_present.reset();
	DX11::hook_resizebuffers.reset();
	DX11::hook_createswapchain.reset();

}