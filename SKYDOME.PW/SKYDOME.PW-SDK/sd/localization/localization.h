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
	}

	__forceinline void setcn() {
		SET_TEXT("console_skydome", "SKYDOME.PW 控制台");
		SET_TEXT("console_skydome_debug", "SKYDOME.PW CONSOLE - 调试");
		SET_TEXT("console_log", "%datetime{%H:%m:%s} -> %level -> %msg");

		SET_TEXT("mem_module_base_not_found",U8ST("模块搜索失败:"));
		SET_TEXT("mem_pattern_not_found", U8ST("特征码搜素失败:"));
		SET_TEXT("mem_export_not_found", U8ST("导出表搜索失败:"));
	}

	std::unordered_map<std::string, std::string> texts;
private:


	

};

inline CheatLocalization* g_CheatLocalization = new CheatLocalization;