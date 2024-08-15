#pragma once

// used: call virtual function
#include "../global.h"

// forward declarations
struct IDXGISwapChain;

class ISwapChainDx11
{
	MEM_PAD(0x170);
	IDXGISwapChain* pDXGISwapChain;
};
