#include"OffsetManager.h"
#include"gobal.h"
#include "sd/utilities/memory.h"

#include <chrono>
bool OffsetManager::scan()
{
	auto start = std::chrono::system_clock::now();
	bool rt = true;
	//���⴦��swapchain
	//https://www.unknowncheats.me/forum/4089361-post4621.html

	auto swapchain = MEM::FindPattern(RENDERSYSTEM_DLL, XorStr("66 0F 7F 05 ?? ?? ?? ?? 48 89 2D ?? ?? ?? ?? 66 0F 7F 0D"));

	if (!swapchain)
	{
		LOG(ERROR) << g_CheatLocalization->get(XorStr("interfaces_swapchain_not_found"));
		return false;
	}

	do
	{
		swapchain++;
	} while (*(uint16_t*)swapchain != 0x0F66);


	offsets[OFFSET_SWAPCHAINDX11] = **reinterpret_cast<ISwapChainDx11***>(MEM::ResolveRelativeAddress(swapchain, 0x4, 0x8));














	for (int i = 0; i < OFFSET_MAX; i++) {
		if (!offsets[i]) {
			LOG(ERROR) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_check")), i, offsets[i]);
			rt = false;
		}
		else{
			LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_check")), i, offsets[i]);
		}

	}

	if (rt){
		// ��ȡ����ʱ��
		auto end = std::chrono::system_clock::now();
		// ����ʱ����λΪ��
		std::chrono::duration<double> elapsed = end - start;
		// ������
		LOG(INFO) << SDlib.StrSystem().printf(g_CheatLocalization->get(XorStr("offset_manager_scan_success")), OFFSET_MAX, elapsed.count());
	}
	else {



	}

	



	return rt;


}
