#pragma once
#include <corecrt_malloc.h>

#include "../../valvesdk.h"
#include "../../OffsetManager.h"
#include <d3d11.h>

#define INTERFACES_INITLOG(NAME,PTR) if(!!PTR){LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);}	\
									else{LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);};		\

class InterfacesManager {
public:
	inline int CheckNull();

	bool init();

	

	ISwapChainDx11* SwapChainDx11;	//��Ϸ�Ľ�����
	IDXGISwapChain* SwapChain;	//�����Լ������Ľ�����
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;

	CInputSystem* InputSystem = nullptr;
	CCSGOInput* CSGOInput = nullptr;
};

inline InterfacesManager* g_interfaces = new InterfacesManager;