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
		SET_TEXT("console_skydome", "SKYDOME.PW 控制台");
		SET_TEXT("console_skydome_debug", "SKYDOME.PW CONSOLE - 调试");
		SET_TEXT("console_log", "%datetime{%H:%m:%s} -> %level -> %msg");
		SET_TEXT("console_start", "晚上好，舰长。看起来初始化线程被成功启动了。");
		SET_TEXT("console_start1", "让我们开始吧。");

		SET_TEXT("mem_module_base_not_found","模块搜索失败:");
		SET_TEXT("mem_pattern_not_found", "特征码搜素失败:");
		SET_TEXT("mem_export_not_found", "导出表搜索失败:");
		SET_TEXT("mem_export_no_export", "模块没有导出表:");

		SET_TEXT("hook_log", "已经挂钩%s");

		SET_TEXT("interfaces_swapchain_not_found", "交换链特征搜索失败！");
		SET_TEXT("interfaces_init_log", "%s -> %p");
		SET_TEXT("interfaces_init_success", "对接口的分段检查已经完成，一切顺利，舰长。");
		SET_TEXT("interfaces_init_fail", "对接口的分段检查失败,第一处空白位于 -> %d");

		SET_TEXT("offset_manager_scan_success", "我们通过特征码扫描了 %d 个地址 ！ 用时%.2f s。");
		SET_TEXT("offset_manager_scan_fail", "抱歉，发生了一些意料之外的错误... %d 个地址中的 %d 个搜索失败了。");
		SET_TEXT("offset_manager_scan_check", "offsets[%d] -> %p");

		SET_TEXT("offset_manager_export_scan_success", "导出表搜索完毕！");
		SET_TEXT("offset_manager_export_scan_fail", "导出表搜索失败！");
	}

	std::unordered_map<std::string, std::string> texts;
private:


	

};

inline CheatLocalization* g_CheatLocalization = new CheatLocalization;