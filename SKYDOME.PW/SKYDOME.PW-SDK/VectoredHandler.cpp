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
	exceptionCause << (const char*)u8"��������Ϸ�в�����һ���쳣...\n����������";

	switch (exceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_IN_PAGE_ERROR:
	{
		exceptionCause << (const char*)u8"���ʳ�ͻ" << std::endl;

		auto exceptionInfo0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
		auto exceptionInfo1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];

		if (!exceptionInfo0)
			exceptionCause << (const char*)u8"��ͼ��ȡ: 0x" << (void*)exceptionInfo1;
		else if (exceptionInfo0 == 1)
			exceptionCause << (const char*)u8"��ͼд��: 0x" << (void*)exceptionInfo1;
		else if (exceptionInfo0 == 8)
			exceptionCause << (const char*)u8"����ִ�б��� (DEP) at: 0x" << (void*)std::hex << exceptionInfo1;
		else
			exceptionCause << (const char*)u8"δ֪���ʳ�ͻλ��: 0x" << (void*)exceptionInfo1;

		break;
	}
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		exceptionCause << (const char*)u8"����Խ��";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		exceptionCause << (const char*)u8"��������δ����";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		exceptionCause << (const char*)u8"�����������";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		exceptionCause << (const char*)u8"����(float)";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		exceptionCause << (const char*)u8"����(int)";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		exceptionCause << (const char*)u8"��׼ȷ�Ľ��";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		exceptionCause << (const char*)u8"��Ч����";
		break;
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_INT_OVERFLOW:
		exceptionCause << (const char*)u8"��ֵ���";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		exceptionCause << (const char*)u8"��ֵ����";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		exceptionCause << (const char*)u8"��ջ���";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		exceptionCause << (const char*)u8"�Ƿ�ָ��";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		exceptionCause << (const char*)u8"��Ч����";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		exceptionCause << (const char*)u8"���ɳ������쳣";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		exceptionCause << (const char*)u8"��Ȩָ��";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		exceptionCause << (const char*)u8"��ջ���";
		break;
	default:
		exceptionCause << (const char*)u8"δ֪";
		break;
	}

	void* exceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

	HMODULE crashedModuleHandle;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, static_cast<LPCSTR>(exceptionAddress), &crashedModuleHandle);
	//Ӧ�ÿ����ж��Ƿ�Ϊ�������

	MODULEINFO crashedModuleInfo;
	GetModuleInformation(GetCurrentProcess(), crashedModuleHandle, &crashedModuleInfo, sizeof(crashedModuleInfo));

	char crashedModuleFullName[MAX_PATH];
	GetModuleFileNameExA(GetCurrentProcess(), crashedModuleHandle, crashedModuleFullName, MAX_PATH);
	char* crashedModuleName = strrchr(crashedModuleFullName, '\\') + 1;

	DWORD64 crashedModuleOffset = ((DWORD64)exceptionAddress) - ((DWORD64)crashedModuleInfo.lpBaseOfDll);

	if (crashedModuleHandle == g_CheatData->self_handle) {
		exceptionCause << (const char*)u8"\n�������쳣����������������ַ��0x" << std::hex << crashedModuleOffset << (const char* )u8"\n����������,�԰�?�����ǻ�˵����...����������Ų����.\n";
	}
	else
		exceptionCause << (const char*)u8"\n�������쳣������" << crashedModuleName << " : 0x" << std::hex << crashedModuleOffset << (const char*)u8"\n�ܸ������������ǵ�ģ����,���ǻ�˵����...������ô˵�����ǵ�����Ϸ�����İ�?�ܱ�Ǹ���ܲ鿴��ջ����...\n";

	exceptionCause << (const char*)u8"\n����һ�¼Ĵ�����...��������û��ʲô��..." << "\n";
	CONTEXT* context = ExceptionInfo->ContextRecord;
	exceptionCause << (const char*)u8"�Ĵ���ת��:" << std::endl;
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


	exceptionCause << (const char*)u8"\n�������ܲ��������,������������޸�(ʵ������������Ҳ��ȷ��)";
	exceptionCause << std::endl;
	//MessageBoxA(0, std::to_string(long long(ExceptionInfo->ExceptionRecord->ExceptionCode)).c_str(), "ENTROPY", MB_OK | MB_ICONERROR);
	//auto msg_wchar = SDlib.StrSystem().Stringtowstring(exceptionCause.str());

	MessageBoxA(NULL, exceptionCause.str().c_str(), "����...���ε���ս��¼�������...��̫˳����", MB_OK | MB_ICONERROR);

	//delete msg_wchar;
	return EXCEPTION_CONTINUE_SEARCH;
}