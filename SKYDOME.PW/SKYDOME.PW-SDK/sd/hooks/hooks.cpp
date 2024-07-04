#include "hooks.h"

#include "../../external/safetyhook/safetyhook.hpp"
#include "../utilities/memory.h"
#include "../interfaces/interfaces.h"

namespace g_hooks {

	namespace DX11 {
		safetyhook::InlineHook hook_present;
		safetyhook::InlineHook hook_resizebuffers;
		safetyhook::InlineHook hook_createswapchain;

	}
}

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


//可能需要缺乏一些必要的检查

HRESULT __stdcall g_hooks::DX11::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{

    return hook_present.call<HRESULT>(pSwapChain, uSyncInterval, uFlags);
}

HRESULT __fastcall g_hooks::DX11::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	CreateRenderTarget();

    return hook_resizebuffers.call<HRESULT>(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
}

HRESULT __stdcall g_hooks::DX11::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	if (g_interfaces->RenderTargetView != nullptr)
		g_interfaces->RenderTargetView->Release();
	g_interfaces->RenderTargetView = nullptr;

	return hook_createswapchain.call<HRESULT>(pFactory, pDevice, pDesc, ppSwapChain);
}




long g_hooks::DX11::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
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
