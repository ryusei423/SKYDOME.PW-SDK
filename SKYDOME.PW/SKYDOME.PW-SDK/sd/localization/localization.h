#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include "../../gobal.h"

#define SET_TEXT(k,t) set(XorStr(k) , XorStr(t));

class CheatLocalization
{
public:
	__forceinline void set(std::string key, std::string text) {
		texts[key] = text;
	};
	__forceinline bool get(std::string key, std::string& text) {
		auto it = texts.find(key);
		if (it != texts.end()) {
			text = it->second;
			return true;
		}
		else {
			return false;
		}

		return false;
	};

	__forceinline std::string get(std::string key) {
		auto it = texts.find(key);
		if (it != texts.end()) {
			return it->second;
		}
		else {
			return "unknown";
		}

		return "unknown";
	};

	__forceinline void seteng() {
		SET_TEXT("console_skydome", "SKYDOME.PW CONSOLE");
		SET_TEXT("console_skydome_debug", "SKYDOME.PW CONSOLE - DEBUG");
		SET_TEXT("console_log", "%datetime{%H:%m:%s} -> %level -> %msg");


		SET_TEXT("mem_module_base_not_found", "module base not found:");
		SET_TEXT("mem_pattern_not_found", "pattern not found:");
		SET_TEXT("mem_export_not_found", "export not found:");

		SET_TEXT("interfaces_swapchain_not_found", "Swapchain signature not found!");

		SET_TEXT("offset_manager_scan_success", "Finded %d sig in %.2f s.");
		SET_TEXT("offset_manager_scan_fail", "offsets[%d] scan failed!");
	}

	__forceinline void setcn() {
		SET_TEXT("console_skydome", "SKYDOME.PW ����̨");
		SET_TEXT("console_skydome_debug", "SKYDOME.PW CONSOLE - ����");
		SET_TEXT("console_log", "%datetime{%H:%m:%s} -> %level -> %msg");
		SET_TEXT("console_start", "���Ϻã���������������ʼ���̱߳��ɹ������ˡ�");
		SET_TEXT("console_start1", "�����ǿ�ʼ�ɡ�");

		SET_TEXT("mem_module_base_not_found","ģ������ʧ��:");
		SET_TEXT("mem_pattern_not_found", "����������ʧ��:");
		SET_TEXT("mem_export_not_found", "����������ʧ��:");
		SET_TEXT("mem_export_no_export", "ģ��û�е�����:");

		SET_TEXT("hook_log", "�Ѿ��ҹ�%s");

		SET_TEXT("interfaces_swapchain_not_found", "��������������ʧ�ܣ�");
		SET_TEXT("interfaces_init_log", "%s -> %p");
		SET_TEXT("interfaces_init_success", "�Խӿڵķֶμ���Ѿ���ɣ�һ��˳����������");
		SET_TEXT("interfaces_init_fail", "�Խӿڵķֶμ��ʧ��,��һ���հ�λ�� -> %d");

		SET_TEXT("offset_manager_scan_success", "����ͨ��������ɨ���� %d ����ַ �� ��ʱ%.2f s��");
		SET_TEXT("offset_manager_scan_fail", "��Ǹ��������һЩ����֮��Ĵ���... %d ����ַ�е� %d ������ʧ���ˡ�");
		SET_TEXT("offset_manager_scan_check", "offsets[%d] -> %p");

		SET_TEXT("offset_manager_export_scan_success", "������������ϣ�");
		SET_TEXT("offset_manager_export_scan_fail", "����������ʧ�ܣ�");
	}

	std::unordered_map<std::string, std::string> texts;
private:


	

};

inline CheatLocalization* g_CheatLocalization = new CheatLocalization;