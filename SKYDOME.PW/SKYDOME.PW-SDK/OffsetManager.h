#pragma once

//����ʹ��a2xƫ�ƵĴ��붼Ӧ�����ñ��ļ�
#include "a2x/offsets.hpp"
#include "a2x/client.dll.hpp"

//ʵ�������ڹ��������������ĺ�����ַ
class OffsetManager
{
public:
	bool scan();
	bool scan_export();

	//�����Ȱ��������������Ľӿڷ������Ȼ���ڳ�ʼ���ӿ�ʱ�������ȡ
	enum
	{

		OFFSET_SWAPCHAINDX11,
		OFFSET_CCSGOINPUT,
		OFFSET_RELATIVE_MODE_MOUSE,
		OFFSET_GET_ENTITY_BY_INDEX,
		OFFSET_GET_BONE,
		OFFSET_GET_MATRIX_FOR_VIEW,
		OFFSET_FRAME_STAGE_NOTIFY,
		OFFSET_GET_FUNCTION,
		OFFSET_GET_FUNCTION1,
		OFFSET_CREATE_MATERIAL,
		OFFSET_CreateMaterialResource,


		OFFSET_GET_HITBOX_SET,

		OFFSET_TRACE,
		OFFSET_TRACE_InitializeTraceInfo,
		OFFSET_TRACE_InitializeTrace,
		OFFSET_TRACE_Init,
		OFFSET_TRACE_ClipTraceToPlayers,
		OFFSET_TRACE_get_trace_info,
		OFFSET_TRACE_handle_bullet_penetration,
		OFFSET_TRACE_CreateTrace,
		OFFSET_TRACE_TraceShape,

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