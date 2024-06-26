#include "VectoredHandler.h"



LONG WINAPI VectoredHandler1::VectoredHandler(_EXCEPTION_POINTERS* ExceptionInfo)
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
		exceptionCause << (const char*)u8"\n看起来异常发生于我们自身，地址是0x" << std::hex << crashedModuleOffset << (const char* )u8"\n很糟糕的事情,对吧?，但是话说回来...这更有助于排查错误.\n";
	}
	else
		exceptionCause << (const char*)u8"\n看起来异常发生于" << crashedModuleName << " : 0x" << std::hex << crashedModuleOffset << (const char*)u8"\n很高兴它不在我们的模块中,但是话说回来...不管怎么说是我们导致游戏崩掉的吧?很抱歉不能查看堆栈调用...\n";

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


	exceptionCause << (const char*)u8"\n建议向总部报告错误,这可能有助于修复(实际上我想他们也不确定)";
	exceptionCause << std::endl;
	//MessageBoxA(0, std::to_string(long long(ExceptionInfo->ExceptionRecord->ExceptionCode)).c_str(), "ENTROPY", MB_OK | MB_ICONERROR);
	//auto msg_wchar = SDlib.StrSystem().Stringtowstring(exceptionCause.str());

	MessageBoxA(NULL, exceptionCause.str().c_str(), "舰长...本次的作战记录整理好了...不太顺利呢", MB_OK | MB_ICONERROR);

	//delete msg_wchar;
	return EXCEPTION_CONTINUE_SEARCH;
}