#pragma once
#include "../interfaces/interfaces.h"
#include "../../external/imgui/imgui.h"

class MenuManager
{
public:
	bool init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);//设置菜单的一些基本信息
	bool create(HWND hWnd);//创建设备和交换链
	void CreateRenderTarget(IDXGISwapChain* pSwapChain);//创建渲染目标
	void frame(IDXGISwapChain* pSwapChain);//不需额外处理，只需调用宾狗和结束
	void toggle(bool state);

	bool show_menu = false;
	int toggle_mouse = 0;

	int cur_tab = 0;
	ImColor color = ImColor(9.f / 255.f,104.f / 255.f,247.f / 255.f);

private:
	void ShowRage();
	void ShowEsp();
	void ShowMisc();
	void make_header(std::string name,const char* font = "main_small");


	std::string logo_text = "SKYDOME.PW";
	std::string logo_text_d = logo_text;
	std::vector<std::string> logo_texts;
	void on_menu_open();
	void on_anim_frame();


	const ImVec2 menu_size_origin = ImVec2(559,700);
	ImVec2 menu_size_scale;

	const ImVec2 tab_size_origin = ImVec2(200, 700);
	ImVec2 tab_size_scale;

	const ImVec2 logo_size_origin = ImVec2(559, 39);
	ImVec2 logo_size_scale;

	ImVec2 child_windows_size;

	ImGuiStyle DefaultStyle;

	int dpi = 1;
	void TryFindDpi();
	inline void UpdateDpi() {
		static int save_dpi = dpi;

		switch (dpi)
		{
			case 0: menu_dpi_scale = 0.75; break;
			case 1: menu_dpi_scale = 1.0f; break;
			case 2: menu_dpi_scale = 1.25f; break;
			case 3: menu_dpi_scale = 1.5f; break;
			case 4: menu_dpi_scale = 2.0f; break;
		}

		if (dpi != save_dpi)
		{
		
			ImGui::GetStyle() = DefaultStyle;
			ImGui::GetStyle().ScaleAllSizes(menu_dpi_scale);
			
			save_dpi = dpi;
		}

		menu_size_scale = menu_size_origin * menu_dpi_scale;
		tab_size_scale = tab_size_origin * menu_dpi_scale;
		logo_size_scale = logo_size_origin * menu_dpi_scale;

		
		child_windows_size.x = menu_size_scale.x / 2 - ImGui::GetStyle().FramePadding.x * 3;


	};
	inline ImFont* GetDpiFont(std::string);
	//我知道这看起来很弱智
	//我会在某个时刻重新设计的
	float menu_dpi_scale_075 = 0.75f;
	float menu_dpi_scale_100 = 1.0f;
	float menu_dpi_scale_125 = 1.25f;
	float menu_dpi_scale_150 = 1.5f;
	float menu_dpi_scale_175 = 1.75f;
	float menu_dpi_scale = menu_dpi_scale_100;


	std::unordered_map<std::string, ImFont*> fonts;


};

inline MenuManager* g_MenuManager = new MenuManager;
