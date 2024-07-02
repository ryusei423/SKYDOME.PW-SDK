#pragma once
#include <corecrt_malloc.h>
#include <winnt.h>

#include "../../valvesdk.h"

class InterfacesManager {
public:
	inline bool CheckNull() {
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
				return false;
			}
		}
		free(buffer);

		return true;
	}

	ISwapChainDx11* SwapChainDx11;


};

inline InterfacesManager* g_interfaces = new InterfacesManager;