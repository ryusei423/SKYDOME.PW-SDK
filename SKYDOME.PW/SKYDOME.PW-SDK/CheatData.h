#pragma once
#include"global.h"

#include"sdk/entity/Entity.h"
class CheatData {
public:
	HMODULE self_handle;

	FILE* m_ConsoleStream;

	CCSPlayerController* LocalController = nullptr;
	C_CSPlayerPawn* LocalPawn = nullptr;
	Vector save_eyepos;
	Vector pred_eyepos;

	bool RelativeLastValue = false;
	bool IsRelative = false;

};

inline CheatData* g_CheatData = new CheatData;