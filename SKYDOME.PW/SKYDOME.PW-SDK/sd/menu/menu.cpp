#include "menu.h"
// used: [ext] imgui
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_freetype.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"

#include "../utilities/memory.h"
#include "../hooks/hooks.h"

//²»ÔÙÓ²±àÂë½»»»Á´
//void CreateRenderTarget(IDXGISwapChain* pDXGISwapChain)
//{
//	if (FAILED(pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_interfaces->Device)))
//	{
//		//L_PRINT(LOG_ERROR) << CS_XOR("failed to get device from swapchain");
//		//CS_ASSERT(false);
//	}
//	else
//		// we successfully got device, so we can get immediate context
//		g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);
//
//	// @note: i dont use this anywhere else so lambda is fine
//	static const auto GetCorrectDXGIFormat = [](DXGI_FORMAT eCurrentFormat)
//		{
//			switch (eCurrentFormat)
//			{
//			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
//				return DXGI_FORMAT_R8G8B8A8_UNORM;
//			}
//
//			return eCurrentFormat;
//		};
//
//	DXGI_SWAP_CHAIN_DESC sd;
//	pDXGISwapChain->GetDesc(&sd);
//
//	ID3D11Texture2D* pBackBuffer = nullptr;
//	if (SUCCEEDED(pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
//	{
//		if (pBackBuffer)
//		{
//			D3D11_RENDER_TARGET_VIEW_DESC desc{};
//			desc.Format = static_cast<DXGI_FORMAT>(GetCorrectDXGIFormat(sd.BufferDesc.Format));
//			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//			if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView)))
//			{
//				//L_PRINT(LOG_WARNING) << CS_XOR("failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D...");
//				//L_PRINT(LOG_INFO) << CS_XOR("retrying to create render target view with D3D11_RTV_DIMENSION_TEXTURE2DMS...");
//				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
//				if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView)))
//				{
//					//L_PRINT(LOG_WARNING) << CS_XOR("failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D...");
//					//L_PRINT(LOG_INFO) << CS_XOR("retrying...");
//					if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, NULL, &g_interfaces->RenderTargetView)))
//					{
//						//L_PRINT(LOG_ERROR) << CS_XOR("failed to create render target view");
//						//CS_ASSERT(false);
//					}
//				}
//			}
//			pBackBuffer->Release();
//			pBackBuffer = nullptr;
//		}
//	}
//}
static auto GetCorrectDXGIFormat(DXGI_FORMAT currentFormat) {
	switch (currentFormat) {
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return currentFormat;
}

static void CreateRenderTarget1(IDXGISwapChain* pSwapChain) {
	//SDK_LOG_PROLOGUE();

	ID3D11Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = GetCorrectDXGIFormat(sd.BufferDesc.Format);

		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView))) {
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView))) {
				if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, NULL, &g_interfaces->RenderTargetView))) {
					//CLogger::Log("[dx11] CreateRenderTargetView() failed!");
				}
			}
		}

		pBackBuffer->Release();
	}
}

//void GetDevice(IDXGISwapChain* pSwapChain) {
//	pSwapChain->GetDevice(IID_PPV_ARGS(&g_interfaces->Device));
//	g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);
//
//}


static bool CreateDevice(HWND hWnd) {

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;

	constexpr D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc,
		&g_interfaces->SwapChain, &g_interfaces->Device, nullptr, nullptr);

	return hr == S_OK;
}

bool MenuManager::init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext){

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = io.LogFilename = nullptr;




	return true;
}

bool MenuManager::create(HWND hWnd)
{
	bool rt = CreateDevice(hWnd);
	//CreateRenderTarget(g_interfaces->SwapChain);
	//GetDevice(g_interfaces->SwapChain);
	return rt;
}

void MenuManager::CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	return CreateRenderTarget1(pSwapChain);
}

void MenuManager::frame(IDXGISwapChain* pSwapChain)
{
	





	
}
