// SKYDOME.PW-SDK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

//SKYDOME.PW - SDK
//2024/6/18
// --------
//初始化时缓存所有偏移
//获取偏移的方法:网络 + 本地
//从网络获取的包括:特征码/索引/当前版本是否可用?

#include <iostream>
#include "external/easylogging/easylogging++.h"
#include "gobal.h"
#include "external/StackWalker/StackWalker.h"

INITIALIZE_EASYLOGGINGPP


LONG WINAPI VectoredHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{

	if (IsDebuggerPresent()) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	const DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	if (exceptionCode != EXCEPTION_ACCESS_VIOLATION && exceptionCode != EXCEPTION_ARRAY_BOUNDS_EXCEEDED &&
		exceptionCode != EXCEPTION_DATATYPE_MISALIGNMENT && exceptionCode != EXCEPTION_FLT_DENORMAL_OPERAND &&
		exceptionCode != EXCEPTION_FLT_DIVIDE_BY_ZERO && exceptionCode != EXCEPTION_FLT_INEXACT_RESULT &&
		exceptionCode != EXCEPTION_FLT_INVALID_OPERATION && exceptionCode != EXCEPTION_FLT_OVERFLOW &&
		exceptionCode != EXCEPTION_FLT_STACK_CHECK && exceptionCode != EXCEPTION_FLT_UNDERFLOW &&
		exceptionCode != EXCEPTION_ILLEGAL_INSTRUCTION && exceptionCode != EXCEPTION_IN_PAGE_ERROR &&
		exceptionCode != EXCEPTION_INT_DIVIDE_BY_ZERO && exceptionCode != EXCEPTION_INT_OVERFLOW &&
		exceptionCode != EXCEPTION_INVALID_DISPOSITION && exceptionCode != EXCEPTION_NONCONTINUABLE_EXCEPTION &&
		exceptionCode != EXCEPTION_PRIV_INSTRUCTION && exceptionCode != EXCEPTION_STACK_OVERFLOW)
		return EXCEPTION_CONTINUE_SEARCH;

	std::stringstream exceptionCause;
	exceptionCause << (const char*)u8"我们在游戏中捕获了一个异常...\n它看起来是";

	switch (exceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_IN_PAGE_ERROR:
	{
		exceptionCause << (const char*)u8"访问冲突" << std::endl;

		auto exceptionInfo0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
		auto exceptionInfo1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];

		if (!exceptionInfo0)
			exceptionCause << (const char*)u8"试图读取: 0x" << (void*)exceptionInfo1;
		else if (exceptionInfo0 == 1)
			exceptionCause << (const char*)u8"试图写入: 0x" << (void*)exceptionInfo1;
		else if (exceptionInfo0 == 8)
			exceptionCause << (const char*)u8"数据执行保护 (DEP) at: 0x" << (void*)std::hex << exceptionInfo1;
		else
			exceptionCause << (const char*)u8"未知访问冲突位于: 0x" << (void*)exceptionInfo1;

		break;
	}
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		exceptionCause << (const char*)u8"数组越界";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		exceptionCause << (const char*)u8"数据类型未对齐";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		exceptionCause << (const char*)u8"非正规操作数";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		exceptionCause << (const char*)u8"除零(float)";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		exceptionCause << (const char*)u8"除零(int)";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		exceptionCause << (const char*)u8"不准确的结果";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		exceptionCause << (const char*)u8"无效操作";
		break;
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_INT_OVERFLOW:
		exceptionCause << (const char*)u8"数值溢出";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		exceptionCause << (const char*)u8"数值下溢";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		exceptionCause << (const char*)u8"堆栈检查";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		exceptionCause << (const char*)u8"非法指令";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		exceptionCause << (const char*)u8"无效配置";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		exceptionCause << (const char*)u8"不可持续的异常";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		exceptionCause << (const char*)u8"特权指令";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		exceptionCause << (const char*)u8"堆栈溢出";
		break;
	default:
		exceptionCause << (const char*)u8"未知";
		break;
	}

	void* exceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

	HMODULE crashedModuleHandle;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, static_cast<LPCSTR>(exceptionAddress), &crashedModuleHandle);
	//应该可以判断是否为自身崩溃

	MODULEINFO crashedModuleInfo;
	GetModuleInformation(GetCurrentProcess(), crashedModuleHandle, &crashedModuleInfo, sizeof(crashedModuleInfo));

	char crashedModuleFullName[MAX_PATH];
	GetModuleFileNameExA(GetCurrentProcess(), crashedModuleHandle, crashedModuleFullName, MAX_PATH);
	char* crashedModuleName = strrchr(crashedModuleFullName, '\\') + 1;

	DWORD64 crashedModuleOffset = ((DWORD64)exceptionAddress) - ((DWORD64)crashedModuleInfo.lpBaseOfDll);

	if (crashedModuleHandle == g_CheatData->self_handle) {
		exceptionCause << (const char*)u8"\n看起来异常发生于我们自身，地址是0x" << std::hex << crashedModuleOffset << (const char*)u8"\n很糟糕的事情,对吧?，但是话说回来...这更有助于排查错误。\n";
	}
	else
		exceptionCause << (const char*)u8"\n看起来异常发生于" << crashedModuleName << " : 0x" << std::hex << crashedModuleOffset << (const char*)u8"\n很高兴它不在我们的模块中,但是话说回来...不管怎么说是我们导致异常发生的吧？很抱歉不能查看堆栈调用...\n";

	exceptionCause << (const char*)u8"\n来看一下寄存器吧...不过我想没有什么用..." << "\n";
	CONTEXT* context = ExceptionInfo->ContextRecord;
	exceptionCause << (const char*)u8"寄存器转储:" << std::endl;
	exceptionCause << (const char*)u8"  RAX: 0x" << std::hex << context->Rax << std::endl;
	exceptionCause << (const char*)u8"  RBX: 0x" << std::hex << context->Rbx << std::endl;
	exceptionCause << (const char*)u8"  RCX: 0x" << std::hex << context->Rcx << std::endl;
	exceptionCause << (const char*)u8"  RDX: 0x" << std::hex << context->Rdx << std::endl;
	exceptionCause << (const char*)u8"  RSI: 0x" << std::hex << context->Rsi << std::endl;
	exceptionCause << (const char*)u8"  RDI: 0x" << std::hex << context->Rdi << std::endl;
	exceptionCause << (const char*)u8"  RBP: 0x" << std::hex << context->Rbp << std::endl;
	exceptionCause << (const char*)u8"  RSP: 0x" << std::hex << context->Rsp << std::endl;
	exceptionCause << (const char*)u8"  RIP: 0x" << std::hex << context->Rip << std::endl;
	exceptionCause << (const char*)u8"  RFLAGS: 0x" << std::hex << context->EFlags << std::endl;

	
	exceptionCause << (const char*)u8"\n请向总部发送此报告,这有助于他们确定发生了什么然后修复此错误。";
	exceptionCause << std::endl;
	
	if (crashedModuleHandle != g_CheatData->self_handle) {
		exceptionCause << (const char*)u8"\n虽然说没有调用堆栈的情况下自身以外的异常十分难以分析，但是提交报告也会带来些许帮助呢。(聊胜于无";
		exceptionCause << std::endl;

	}
	

	//MessageBoxA(0, std::to_string(long long(ExceptionInfo->ExceptionRecord->ExceptionCode)).c_str(), "ENTROPY", MB_OK | MB_ICONERROR);
	//auto msg_wchar = SDlib.StrSystem().Stringtowstring(exceptionCause.str());
	auto msg_wchar = SDlib.StrSystem().Stringtowstring(exceptionCause.str());
	MessageBoxW(NULL, msg_wchar.c_str(), L"舰长...本次的作战记录整理好了...不太顺利呢", MB_OK | MB_ICONERROR);
	//GetCurrentStackTrace(ExceptionInfo->ContextRecord);
	//delete msg_wchar;

	return EXCEPTION_CONTINUE_SEARCH;
}


//TODO:更体面地管理控制台
void CreateConsole() {
	AllocConsole();
	SetConsoleTitleA(g_CheatLocalization->get(XorStr("console_skydome")).c_str());
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);

#ifdef _DEBUG
	SetConsoleTitleA(g_CheatLocalization->get(XorStr("console_skydome_debug")).c_str());
#endif // DEBUG
	
	freopen_s(&g_CheatData->m_ConsoleStream, XorStr("CONOUT$"), XorStr("w"), stdout);

	el::Configurations defaultConf;
	defaultConf.setToDefault();
	// Values are always std::string
	//defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "[SKYDOME.PW] %datetime %level %msg");
	defaultConf.set(el::Level::Global, el::ConfigurationType::Format, g_CheatLocalization->get(XorStr("console_log")));


	defaultConf.parseFromText(XorStr("*GLOBAL:\n TO_FILE = FALSE\n FILENAME = "" \n"));
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

	el::Loggers::reconfigureLogger(XorStr("default"), defaultConf);
	// Actually reconfigure all loggers instead
	el::Loggers::reconfigureAllLoggers(defaultConf);


	/*el::Loggers::getLogger("default")->setFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Loggers::getLogger("default")->setFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Loggers::getLogger("default")->setConfigurations(el::ConfigurationType::Format, "%datetime %level %msg");
	el::Loggers::getLogger("default")->setFlag(el::LoggingFlag::DisableFileLogging);*/
	/*LOG(ERROR) << "test msg";
	LOG(INFO) << "test msg";*/
}





#include "sd/utilities/memory.h"
#include "OffsetManager.h"
#include "sd/interfaces/interfaces.h"
#include "sd/hooks/hooks.h"
uintptr_t __stdcall init_main(const HMODULE h_module) {

	g_CheatLocalization->setcn();
	//g_CheatLocalization->seteng();
	CreateConsole();
	//欢迎信息
	LOG(INFO) << g_CheatLocalization->get(XorStr("console_start"));
	LOG(INFO) << g_CheatLocalization->get(XorStr("console_start1"));

	//不需要
	//MEM::Setup();
	bool rt = true;
	rt = g_OffsetManager->scan();
	SD_ASSERT(rt);
	rt = g_interfaces->init();
	SD_ASSERT(rt);
	rt = g_hooks::init();
	SD_ASSERT(rt);
	
	return 0;
}





int __stdcall DllMain(HMODULE dllhandle,
	unsigned long _Reason, void* _Reserved)
{

	if (_Reason == DLL_PROCESS_ATTACH)
	{
		//将本线程设置为高优先级
		//实际上，不知道它有什么用，是我从fa的源码里捡的
		//有种不要白不要的感觉
		auto current_process = GetCurrentProcess();
		auto priority_class = GetPriorityClass(current_process);

		if (priority_class != HIGH_PRIORITY_CLASS && priority_class != REALTIME_PRIORITY_CLASS)
			SetPriorityClass(current_process, HIGH_PRIORITY_CLASS);


		g_CheatData->self_handle = dllhandle;

		//防止多次调用dllmain
		DisableThreadLibraryCalls(dllhandle);
		//注册异常处理程序
		AddVectoredExceptionHandler(0, VectoredHandler);


		//创建初始化线程
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init_main, dllhandle, 0, nullptr);
		
	}

	if (_Reason == DLL_PROCESS_DETACH) {
	
		g_hooks::unhook();

		HWND consoleWindow = GetConsoleWindow();

		FreeConsole();

		if (consoleWindow != NULL)
		{
			// 发送关闭窗口消息
			PostMessage(consoleWindow, WM_CLOSE, 0, 0);
		}
	}

	return true;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
