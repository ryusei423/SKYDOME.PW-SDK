#pragma once
#include"gobal.h"

class CheatData {
public:
	HMODULE self_handle;

	FILE* m_ConsoleStream;

	


	bool RelativeLastValue = false;

};

inline CheatData* g_CheatData = new CheatData;