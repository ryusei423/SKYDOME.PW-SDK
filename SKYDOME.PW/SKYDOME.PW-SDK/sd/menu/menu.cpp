#include "menu.h"
// used: [ext] imgui
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_freetype.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"

#include "../utilities/memory.h"

void CreateRenderTarget()
{
	if (FAILED(g_interfaces->SwapChainDx11->pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_interfaces->Device)))
	{
		//L_PRINT(LOG_ERROR) << CS_XOR("failed to get device from swapchain");
		//CS_ASSERT(false);
	}
	else
		// we successfully got device, so we can get immediate context
		g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);

	// @note: i dont use this anywhere else so lambda is fine
	static const auto GetCorrectDXGIFormat = [](DXGI_FORMAT eCurrentFormat)
		{
			switch (eCurrentFormat)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			return eCurrentFormat;
		};

	DXGI_SWAP_CHAIN_DESC sd;
	g_interfaces->SwapChainDx11->pDXGISwapChain->GetDesc(&sd);

	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(g_interfaces->SwapChainDx11->pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
	{
		if (pBackBuffer)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = static_cast<DXGI_FORMAT>(GetCorrectDXGIFormat(sd.BufferDesc.Format));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView)))
			{
				//L_PRINT(LOG_WARNING) << CS_XOR("failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D...");
				//L_PRINT(LOG_INFO) << CS_XOR("retrying to create render target view with D3D11_RTV_DIMENSION_TEXTURE2DMS...");
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView)))
				{
					//L_PRINT(LOG_WARNING) << CS_XOR("failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D...");
					//L_PRINT(LOG_INFO) << CS_XOR("retrying...");
					if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, NULL, &g_interfaces->RenderTargetView)))
					{
						//L_PRINT(LOG_ERROR) << CS_XOR("failed to create render target view");
						//CS_ASSERT(false);
					}
				}
			}
			pBackBuffer->Release();
			pBackBuffer = nullptr;
		}
	}
}

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

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = io.LogFilename = nullptr;




	return true;
}

void MenuManager::frame()
{
	if (!ImGui::GetCurrentContext()) return;

	if (!ImGui::GetIO().BackendRendererUserData) {
		//if (SUCCEEDED(g_interfaces->SwapChainDx11->pDXGISwapChain->GetDevice(IID_PPV_ARGS(&g_interfaces->Device)))) {
		//	g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);
		//	ImGui_ImplDX11_Init(g_interfaces->Device, g_interfaces->DeviceContext);

		//	//CRenderer::Get().Initialize();
		//}
		//else {
		//	//return CLogger::Log("[dx11] GetDevice() failed!");
		//	return;
		//}

		return;
	}

	//if (!g_interfaces->RenderTargetView) {
	//	//CreateRenderTarget(pSwapChain);
	//	CreateRenderTarget();
	//}
	//else {
	//	ImGui_ImplDX11_NewFrame();
	//	ImGui_ImplWin32_NewFrame();
	//	ImGui::NewFrame();

	//	//CRenderer::Get().NewFrame();

	//	ImGui::Render();
	//	g_interfaces->DeviceContext->OMSetRenderTargets(1, &g_interfaces->RenderTargetView, NULL);
	//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//}





	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("SKYDOME");


	ImGui::End();

	ImGui::Render();
	g_interfaces->DeviceContext->OMSetRenderTargets(1, &g_interfaces->RenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void MenuManager::on_resizebuffers(){
	CreateRenderTarget();

}

void MenuManager::on_createswapchain(){


}
