#pragma once
#include <corecrt_malloc.h>

#include "../../valvesdk.h"
#include "../../OffsetManager.h"
#include <d3d11.h>

#define INTERFACES_INITLOG(NAME,PTR) if(!!PTR){LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);}	\
									else{check = false;LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);};		\

class InterfacesManager {
public:

	bool init();

	ISwapChainDx11* GameSwapChain = nullptr;
	IDXGISwapChain* SwapChain = nullptr;
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;

	CInputSystem* InputSystem = nullptr;
	CCSGOInput* CSGOInput = nullptr;
	IMemAlloc* MemAlloc = nullptr;
	ISchemaSystem* SchemaSystem = nullptr;
	CGameEntitySystem* GameEntitySystem = nullptr;
	IGameResourceService* GameResourceService = nullptr;
	IEngineClient* EngineClient = nullptr;
	i_trace* Trace = nullptr;
	IEngineCVar* EngineCVar = nullptr;
	IGlobalVars* GlobalVars = nullptr;
	IClient* Client = nullptr;
	INetworkClientService* NetworkClientService = nullptr;

};

inline InterfacesManager* g_interfaces = new InterfacesManager;