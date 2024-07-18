#pragma once
#include"gobal.h"
#include"sdk/datatypes/matrix.h"

class CheatData {
public:
	HMODULE self_handle;

	FILE* m_ConsoleStream;

	Matrix3x4_t* matrix;


	bool RelativeLastValue = false;

};

inline CheatData* g_CheatData = new CheatData;