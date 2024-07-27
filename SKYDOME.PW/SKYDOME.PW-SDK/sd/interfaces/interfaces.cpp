#include "interfaces.h"
#include "../utilities/memory.h"
#include"../utilities/crt.h"

inline int InterfacesManager::CheckNull()
{
	auto size = sizeof(InterfacesManager);
	auto ptr_size = sizeof(void*);
	auto cout = ptr_size / size;
	auto buffer = malloc(size);
	memcpy(buffer, this, size);
	for (int i = 0; i < cout; i++) {
		int offset = i * ptr_size;
		void* cur = this + offset;

		if (!*(void**)cur)
		{
			return i;
		}
	}
	free(buffer);

	return 1337;
}

using InstantiateInterfaceFn_t = void* (*)();

class CInterfaceRegister
{
public:
	InstantiateInterfaceFn_t fnCreate;
	const char* szName;
	CInterfaceRegister* pNext;
};

static const CInterfaceRegister* GetRegisterList(const wchar_t* wszModuleName)
{
	void* hModule = MEM::GetModuleBaseHandle(wszModuleName);
	if (hModule == nullptr)
		return nullptr;

	std::uint8_t* pCreateInterface = reinterpret_cast<std::uint8_t*>(MEM::GetExportAddress(hModule, XorStr("CreateInterface")));

	if (pCreateInterface == nullptr)
	{
		LOG(ERROR) << XorStr("failed to get \"CreateInterface\" address");
		return nullptr;
	}

	return *reinterpret_cast<CInterfaceRegister**>(MEM::ResolveRelativeAddress(pCreateInterface, 0x3, 0x7));
}

template <typename T = void*>
T* Capture(const CInterfaceRegister* pModuleRegister, const char* szInterfaceName)
{
	for (const CInterfaceRegister* pRegister = pModuleRegister; pRegister != nullptr; pRegister = pRegister->pNext)
	{
		if (const std::size_t nInterfaceNameLength = CRT::StringLength(szInterfaceName);
			// found needed interface
			CRT::StringCompareN(szInterfaceName, pRegister->szName, nInterfaceNameLength) == 0 &&
			// and we've given full name with hardcoded digits
			(CRT::StringLength(pRegister->szName) == nInterfaceNameLength ||
				// or it contains digits after name
				CRT::StringToInteger<int>(pRegister->szName + nInterfaceNameLength, nullptr, 10) > 0))
		{
			// capture our interface
			void* pInterface = pRegister->fnCreate();

#ifdef _DEBUG
			// log interface address
			LOG(INFO) << XorStr("captured \"") << pRegister->szName << XorStr("\" interface at address: ") << std::hex << reinterpret_cast<std::uintptr_t>(pInterface);
#else
			LOG(INFO) << XorStr("captured \"") << pRegister->szName << XorStr("\" interface");
#endif

			return static_cast<T*>(pInterface);
		}
	}

	LOG(INFO) << XorStr("failed to find interface \"") << szInterfaceName << XorStr("\"");
	return nullptr;
}



bool InterfacesManager::init()
{
	/*SwapChainDx11 = reinterpret_cast<ISwapChainDx11*>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_SWAPCHAINDX11]);
	INTERFACES_INITLOG("SwapChainDx11", SwapChainDx11);
	if (SwapChainDx11) {
		SwapChainDx11->pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Device);
		INTERFACES_INITLOG("Device", Device);
	}

	if (Device) {
		Device->GetImmediateContext((ID3D11DeviceContext**)&DeviceContext);
		INTERFACES_INITLOG("DeviceContext", DeviceContext);
	}*/
	const auto pTier0Handle = MEM::GetModuleBaseHandle(TIER0_DLL);

	const auto pInputSystemRegisterList = GetRegisterList(INPUTSYSTEM_DLL);
	const auto pSchemaSystemRegisterList = GetRegisterList(SCHEMASYSTEM_DLL);
	const auto pEngineRegisterList = GetRegisterList(ENGINE2_DLL);
	const auto pTier0RegisterList = GetRegisterList(TIER0_DLL);
	const auto pClientRegisterList = GetRegisterList(CLIENT_DLL);

	MemAlloc = *reinterpret_cast<IMemAlloc**>(MEM::GetExportAddress(pTier0Handle, XorStr("g_pMemAlloc")));
	CSGOInput = reinterpret_cast<CCSGOInput*>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_CCSGOINPUT]);
	Trace = reinterpret_cast<i_trace*>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_TRACE]);
	GlobalVars = reinterpret_cast<IGlobalVars*>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GOBALVARS]);
	InputSystem = Capture<CInputSystem>(pInputSystemRegisterList, XorStr("InputSystemVersion00"));
	SchemaSystem = Capture<ISchemaSystem>(pSchemaSystemRegisterList, XorStr("SchemaSystem_00"));
	GameResourceService = Capture<IGameResourceService>(pEngineRegisterList, XorStr("GameResourceServiceClientV00"));
	EngineClient = Capture<IEngineClient>(pEngineRegisterList, XorStr("Source2EngineToClient00"));
	EngineCVar = Capture<IEngineCVar>(pTier0RegisterList, XorStr("VEngineCvar00"));
	Client = Capture<IClient>(pClientRegisterList, XorStr("Source2Client00"));

	INTERFACES_INITLOG("CSGOInput", CSGOInput);
	INTERFACES_INITLOG("Trace", Trace);
	INTERFACES_INITLOG("InputSystem", InputSystem);
	
	INTERFACES_INITLOG("SchemaSystem", SchemaSystem);
	INTERFACES_INITLOG("GameResourceService", GameResourceService);
	INTERFACES_INITLOG("EngineClient", EngineClient);
	INTERFACES_INITLOG("MemAlloc", MemAlloc);
	INTERFACES_INITLOG("EngineCVar", EngineCVar);
	if (CheckNull() != 1337) {

		LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("interfaces_init_fail")), CheckNull());
		return false;
	}


	LOG(INFO) << g_CheatLocalization->get(XorStr("interfaces_init_success"));
	return true;
}
