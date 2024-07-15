#pragma once


//实际上用于管理特征码搜索的函数地址
class OffsetManager
{
public:
	bool scan();
	bool scan_export();

	//我们先把用特征码搜索的接口放在这里，然后在初始化接口时从这里读取
	enum
	{

		OFFSET_SWAPCHAINDX11,
		OFFSET_CCSGOINPUT,
		OFFSET_RELATIVE_MODE_MOUSE,
		OFFSET_MAX
	};

	int(__stdcall* fnSetRelativeMouseMode)(int) = nullptr;
	int(__stdcall* fnSetWindowMouseGrab)(void*, int) = nullptr;
	bool(__stdcall* fnGetRelativeMouseMode)(void) = nullptr;
	int(__stdcall* fnWarpMouseInWindow)(void*, float, float) = nullptr;
	int(__stdcall* fnShowCursor)(int) = nullptr;
	/*bool(__fastcall* fnLoadKV3)(CKeyValues3*, void*, const char*, const KV3ID_t*, const char*);
	std::int64_t(__fastcall* fnCreateMaterial)(void*, void*, const char*, void*, unsigned int, unsigned int);
	void(__fastcall* fnUtlBufferInit)(CUtlBuffer*, int, int, int);
	void(__fastcall* fnUtlBufferPutString)(CUtlBuffer*, const char*);
	void(__fastcall* fnUtlBufferEnsureCapacity)(CUtlBuffer*, int);*/




	void* offsets[OFFSET_MAX];

};

inline OffsetManager* g_OffsetManager = new OffsetManager;