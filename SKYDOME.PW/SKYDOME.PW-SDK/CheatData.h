#pragma once
#include"gobal.h"

#include"sdk/entity/Entity.h"
class CheatData {
public:
	HMODULE self_handle;

	FILE* m_ConsoleStream;

	CCSPlayerController* LocalController = nullptr;
	C_CSPlayerPawn* LocalPawn = nullptr;
	Vector net_update_end_eyepos;

	bool RelativeLastValue = false;
	bool IsRelative = false;

};

inline CheatData* g_CheatData = new CheatData;