#pragma once
#include"../../gobal.h"
#include <d3d11.h>

namespace g_hooks {
	bool init();
	void unhook();

	namespace DX11 {
		inline HWND hwnd = NULL;
		HRESULT WINAPI Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
		HRESULT __fastcall ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
		HRESULT WINAPI CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
		
		

	}
	




}