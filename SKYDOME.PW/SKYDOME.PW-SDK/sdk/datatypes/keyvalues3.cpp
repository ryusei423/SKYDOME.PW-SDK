#include "keyvalue3.h"
// used: CRT
#include "../../sd/utilities/crt.h"
// used: utlbuffer
#include "utlbuffer.h"

void CKeyValues3::LoadFromBuffer(const char* szString)
{
	CUtlBuffer buffer(0, (CRT::StringLength(szString) + 10), 1);
	buffer.PutString(szString);
	LoadKV3(&buffer);
}

bool CKeyValues3::LoadKV3(CUtlBuffer* buffer)
{
	using fnLoadKeyValues = bool(__fastcall*)(CKeyValues3*, void*, CUtlBuffer*, KV3ID_t*, void*, void*, void*, void*, const char*);
	static const fnLoadKeyValues oLoadKeyValues = reinterpret_cast<fnLoadKeyValues>(MEM::GetAbsoluteAddress(MEM::FindPattern(TIER0_DLL, XorStr("E8 ? ? ? ? EB 36 8B 43 10")), 0x1, 0x0));

	#ifdef CS_PARANOID
	CS_ASSERT(oLoadKeyValues != nullptr);
	#endif

	const char* szName = XorStr("");
	KV3ID_t kv3ID = KV3ID_t(XorStr("generic"), 0x41B818518343427E, 0xB5F447C23C0CDF8C);
	return oLoadKeyValues(this, nullptr, buffer, &kv3ID, nullptr, nullptr, nullptr, nullptr, XorStr(""));
}
