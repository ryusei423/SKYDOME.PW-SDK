#pragma once
#include <corecrt_malloc.h>
#include <winnt.h>

#include "../../valvesdk.h"
#include "../../OffsetManager.h"
#include <d3d11.h>

#define INTERFACES_INITLOG(NAME,PTR) if(!!PTR){LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);}	\
									else{LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_log")),NAME,PTR);};		\

class InterfacesManager {
public:
	inline int CheckNull() {
		auto size = sizeof(InterfacesManager);
		auto ptr_size = sizeof(void*);
		auto cout = ptr_size / size;
		auto buffer = malloc(size);
		memcpy(buffer, this, size);
		for (int i = 0; i < cout; i++){
			int offset = i * ptr_size;
			void* cur = this + offset;

			if (!*(int*)cur)
			{
				return i;
			}
		}
		free(buffer);

		return 1337;
	}

	bool init() {
		SwapChainDx11 = reinterpret_cast<ISwapChainDx11*>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_SWAPCHAINDX11]);
		INTERFACES_INITLOG("SwapChainDx11", SwapChainDx11);
	

		

		if (SwapChainDx11) {
			SwapChainDx11->pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Device);
			INTERFACES_INITLOG("Device", Device);
		}
			
		if (Device) {
			Device->GetImmediateContext((ID3D11DeviceContext**)&DeviceContext);
			INTERFACES_INITLOG("DeviceContext", DeviceContext);
		}
		
	
		if (CheckNull() != 1337){

			LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_fail")), CheckNull());
			return false;
		}


		LOG(INFO) << g_CheatLocalization->get(XorStr("interfaces_init_success"));
		return true;
	}

	ISwapChainDx11* SwapChainDx11;
	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;


};

inline InterfacesManager* g_interfaces = new InterfacesManager;