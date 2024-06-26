#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <sstream>
#include <iostream>
#include "external/SD.h"

class CheatData {
public:
	HMODULE self_handle;

	FILE* m_ConsoleStream;


};

inline CheatData* g_CheatData = new CheatData;