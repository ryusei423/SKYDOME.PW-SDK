#include "menu.h"

#include "../../external/imgui/imgui_freetype.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_internal.h"

#include "../utilities/memory.h"
#include "../hooks/hooks.h"

#include "widget.h"
#include "config.h"


static auto GetCorrectDXGIFormat(DXGI_FORMAT currentFormat) {
	switch (currentFormat) {
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	return currentFormat;
}

static void CreateRenderTarget1(IDXGISwapChain* pSwapChain) {
	//SDK_LOG_PROLOGUE();

	ID3D11Texture2D* pBackBuffer = NULL;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer) {
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = GetCorrectDXGIFormat(sd.BufferDesc.Format);

		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView))) {
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, &desc, &g_interfaces->RenderTargetView))) {
				if (FAILED(g_interfaces->Device->CreateRenderTargetView(pBackBuffer, NULL, &g_interfaces->RenderTargetView))) {
					//CLogger::Log("[dx11] CreateRenderTargetView() failed!");
				}
			}
		}

		pBackBuffer->Release();
	}
}

//void GetDevice(IDXGISwapChain* pSwapChain) {
//	pSwapChain->GetDevice(IID_PPV_ARGS(&g_interfaces->Device));
//	g_interfaces->Device->GetImmediateContext(&g_interfaces->DeviceContext);
//
//}


static bool CreateDevice(HWND hWnd) {

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;

	constexpr D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc,
		&g_interfaces->SwapChain, &g_interfaces->Device, nullptr, nullptr);

	return hr == S_OK;
}


void EditStyle() {


	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;
	colors[ImGuiCol_ChildBg] = ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.0f);
	colors[ImGuiCol_WindowBg] = ImVec4(28.f / 255.f, 28.f / 255.f, 28.f / 255.f, 1.0f);

	colors[ImGuiCol_Button] = ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 1.0f);

	colors[ImGuiCol_Border] = ImVec4(36.f / 255.f, 36.f / 255.f, 36.f / 255.f, 150.f / 255.f);
}

void MenuManager::TryFindDpi() {
	auto size = ImGui::GetWindowSize();

	if (size == ImVec2(1920,1080)){
		dpi = 0;
	}

	if (size == ImVec2(2560, 1440)) {
		dpi = 1;
	}

	if (size == ImVec2(3840, 2160)) {
		dpi = 2;
	}

}

#include "chinese.h"
#include "fonts/Roboto-Bold.h"
#include "fonts/icon.h"
bool MenuManager::init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext){

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = io.LogFilename = nullptr;
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	EditStyle();
	DefaultStyle = ImGui::GetStyle();

	//TryFindDpi();

	static ImVector<ImWchar> myRange;
	ImFontGlyphRangesBuilder myGlyph;

	//这个字符范围是imgui自带的 全部中文字符集 除去CJK图形部分的所有东西
	//用这部分加上我们的3500字就很完美
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0,
	};


	myGlyph.AddText((const char*)chinese3500);
	myGlyph.AddRanges(&ranges[0]);//加上默认的文字范围...因为我们也要显示英文
	myGlyph.BuildRanges(&myRange);

	ImFontConfig cfg;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LoadColor | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting;

	//TODO: io.Fonts->Clear();

	fonts.insert(std::make_pair("main_75", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_075, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_small_75", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_075, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_75", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_075)));
	fonts.insert(std::make_pair("icon_75", io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 17.f * menu_dpi_scale_075, &cfg)));

	fonts.insert(std::make_pair("main_100", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_small_100", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_100", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f)));
	fonts.insert(std::make_pair("icon_100", io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 17.f)));

	fonts.insert(std::make_pair("main_125", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_125, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_small_125", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_125, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_125", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_125)));
	fonts.insert(std::make_pair("icon_125", io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 17.f * menu_dpi_scale_125, &cfg)));

	fonts.insert(std::make_pair("main_150", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_150, NULL, io.Fonts->GetGlyphRangesChineseFull()/*myRange.Data*/)));
	fonts.insert(std::make_pair("main_small_150", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_150, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_150", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_150)));
	fonts.insert(std::make_pair("icon_150", io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 17.f * menu_dpi_scale_150, &cfg)));

	fonts.insert(std::make_pair("main_175", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_175, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_small_175", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_175, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_175", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_175)));
	fonts.insert(std::make_pair("icon_175", io.Fonts->AddFontFromMemoryTTF(icon, sizeof(icon), 17.f * menu_dpi_scale_175, &cfg)));


	logo_texts.push_back(U8ST("花无凋零之日 / 意无传达之时"));
	//logo_texts.push_back(U8ST("故事总是拥有美好的开始 / 却总是难以得到美好的结局"));
	logo_texts.push_back(U8ST("美艳不可方物 / 清丽可涤尘世"));

	return true;
}

bool MenuManager::create(HWND hWnd)
{
	bool rt = CreateDevice(hWnd);
	//CreateRenderTarget(g_interfaces->SwapChain);
	//GetDevice(g_interfaces->SwapChain);
	return rt;
}

void MenuManager::CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	return CreateRenderTarget1(pSwapChain);
}


inline ImFont* MenuManager::GetDpiFont(std::string font) {
	if (menu_dpi_scale == 0.75f){
		return fonts[font + XorStr("_75")];
	}

	if (menu_dpi_scale == 1.0f) {
		return fonts[font + XorStr("_100")];
	}

	if (menu_dpi_scale == 1.25f) {
		return fonts[font + XorStr("_125")];
	}

	if (menu_dpi_scale == 1.5f) {
		return fonts[font + XorStr("_150")];
	}

	if (menu_dpi_scale == 2.f) {
		return fonts[font + XorStr("_175")];
	}

	return nullptr;

};


void MenuManager::ShowRage(){
	ImGui::Columns(2, nullptr, false);
	ImGui::SetColumnOffset(1, 275.9 * menu_dpi_scale);

	ImGui::BeginChild("ragebot_config", ImVec2(child_windows_size.x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);
	make_header(U8ST("武器参数"));

	
	
	
	ImGui::EndChild();

	ImGui::Spacing();
	


	ImGui::BeginChild("ragebot_aa", ImVec2(child_windows_size.x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);

	make_header(U8ST("反瞄准"));
	ImGuiW::Checkbox(U8ST("启用"), g_ConfigManager->GetBool("antiaim_enable"));

	const char* aa_pitch_str[] = { U8ST("关闭"), U8ST("低头"),U8ST("抬头"),U8ST("零"),U8ST("抖动") };
	ImGui::Combo(U8ST("俯仰"), &g_ConfigManager->configs["ragebot_pitch"].it, aa_pitch_str, IM_ARRAYSIZE(aa_pitch_str));

	const char* aa_yaw_str[] = { U8ST("关闭"), U8ST("向后") };
	ImGui::Combo(U8ST("偏航"), &g_ConfigManager->configs["ragebot_yaw"].it, aa_yaw_str, IM_ARRAYSIZE(aa_yaw_str));

	static int tmp = 50;
	static int min = 100;
	static int max = 0;
	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 8);
	ImGuiW::SliderScalar(U8ST("测试"), ImGuiDataType_S32, &tmp, &min, &max, "%d%%", 0);
	ImGui::EndChild();
	ImGui::NextColumn();




	ImGui::BeginChild("ragebot_set", ImVec2(child_windows_size.x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);

	make_header(U8ST("瞄准选项"));
	ImGuiW::Checkbox(U8ST("启用"), g_ConfigManager->GetBool("ragebot_enable"));
	ImGuiW::Checkbox(U8ST("调试攻击cmd信息"), g_ConfigManager->GetBool("ragebot_debug_cmd_info"));
	ImGui::EndChild();

}

void MenuManager::ShowEsp(){
	ImGui::BeginChild("player_esp", ImVec2(child_windows_size.x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);
	make_header(U8ST("玩家视觉"));

	ImGui::Checkbox(U8ST("启用透视"), g_ConfigManager->GetBool("esp_enable"));
	ImGui::Checkbox(U8ST("玩家方框"), g_ConfigManager->GetBool("esp_box"));

	ImGui::EndChild();
}

bool show_demo_window;
bool show_debug_window;
void MenuManager::ShowMisc(){




	ImGui::BeginChild("config", ImVec2(child_windows_size.x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);

	make_header(U8ST("参数保存"));


	static bool shit = false;
	//ImGui::Checkbox(U8ST("一键开转"), &g_ConfigManager->configs["1"].bl);

	


	ImGui::EndChild();


	ImGui::SameLine();


	ImGui::BeginChild("misc", ImVec2(child_windows_size.x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, /*ImGuiWindowFlags_MenuBar*/0);

	make_header(U8ST("其他设置"));


	const char* dpi_str[] = { "75%", "100%" , "125%" , "150%", "175%" };
	ImGui::Combo(U8ST("DPI缩放"), &dpi, dpi_str, IM_ARRAYSIZE(dpi_str));

	#ifdef _DEBUG
		ImGui::Checkbox(U8ST("显示演示窗口"), &show_demo_window);
		ImGui::Checkbox(U8ST("显示调试窗口"), &show_debug_window);
	#endif // _DEBUG

	


	if(ImGui::Button(U8ST("更新GLOBALVARS")))
		g_interfaces->GlobalVars = *reinterpret_cast<IGlobalVars**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL,
			XorStr("48 89 0D ? ? ? ? 48 89 41")), 0x3, 0x7));;


	ImGui::EndChild();




}












#include "anim/ImVec4Anim.h"

static ImVec4 vTextColor(1.0F, 1.0F, 1.0F, 1.0F);
static imanim::ImVec4Anim* pColorAnim = nullptr;

static ImVec4 MenuAlpha(1.0F, 1.0F, 1.0F, 1.0F);
static imanim::ImVec4Anim* MenuAlphaAnim = nullptr;

#include "../../CheatData.h"
void MenuManager::frame(IDXGISwapChain* pSwapChain)
{
	if (!show_menu && g_CheatData->IsRelative)
	{
		g_OffsetManager->fnSetRelativeMouseMode(!show_menu);
		g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
	}

	#ifdef _DEBUG
	
	if(show_demo_window)
		ImGui::ShowDemoWindow();
	#endif // _DEBUG

	

	if (show_menu)
	{
		on_anim_frame();

		UpdateDpi();

		//ImGui::GetIO().MouseDrawCursor = true;
		ImVec2 main_windows_pos;

		ImGui::SetNextWindowSize(menu_size_scale);

		ImGui::GetStyle().Alpha = MenuAlpha.w;
		color.Value.w = MenuAlpha.w;

		ImGui::Begin("SKYDOME",0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

		main_windows_pos = ImGui::GetWindowPos();

		ImGui::SetCursorPos(ImVec2(0, 0));
		if (ImGui::BeginChild("top", logo_size_scale, true, ImGuiWindowFlags_NoDecoration)) {


			
			ImGui::PushFont(GetDpiFont(logo_text_d == logo_text ? "logo" : "main"));

			ImVec2 text_size = ImGui::CalcTextSize("SKYDOME.PW");

			ImGui::SetCursorPos(ImVec2(12 * menu_dpi_scale, (logo_size_scale.y / 2) - (text_size.y / 2)));
			//ImGui::Text(U8ST("SKYDOME.PW"));

			ImGui::TextColored(vTextColor, logo_text_d.c_str());

			
		
			ImGui::PopFont();

		}
		ImGui::EndChild();

		ImGui::GetWindowDrawList()->AddLine(ImGui::GetWindowPos() + ImVec2(0, logo_size_scale.y) - ImVec2(0, 2), ImGui::GetWindowPos() + logo_size_scale - ImVec2(0, 2), color);

		ImVec2 top_l = ImGui::GetWindowPos() + ImVec2(0, logo_size_scale.y);
		ImVec2 top_r = top_l + ImVec2(logo_size_scale.x,0);
		float a = 125.f;
		for (int i = 0; i < 10; i++) {
			auto color = ImGui::GetStyleColorVec4(ImGuiCol_BorderShadow);
			color.w = a / 255.f;
			ImGui::GetWindowDrawList()->AddLine(top_l, top_r, ImGui::ColorConvertFloat4ToU32(color));
			top_l.y += 1;
			top_r.y += 1;
			a -= 12.5f;
		}

		ImGui::ItemSize(ImVec2(0, 4 * menu_dpi_scale));
		ImGui::PushFont(GetDpiFont("main"));



		switch (cur_tab)
		{
			case 1:ShowRage(); break;
			case 2:ShowEsp(); break;
			case 3:ShowMisc(); break;

		default:
			break;
		}
		


		ImGui::PopFont();


		ImGui::End();

		ImGui::SetNextWindowSize(tab_size_scale);
		ImGui::SetNextWindowPos(main_windows_pos + ImVec2(menu_size_scale.x + 8 * menu_dpi_scale, 0));
		ImGui::Begin("SKYDOME_TAB", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
		

		ImGui::SetCursorPos(ImVec2(0, 0));
		if (ImGui::BeginChild("tab_top", logo_size_scale, ImGuiChildFlags_Border, ImGuiWindowFlags_NoDecoration)) {



			ImGui::PushFont(GetDpiFont("icon"));

			ImVec2 text_size = ImGui::CalcTextSize("SKYDOME.PW");

			ImGui::SetCursorPos(ImVec2(12 * menu_dpi_scale, (logo_size_scale.y / 2) - (text_size.y / 2)));
			ImGui::Text(U8ST("G"));
			

			ImGui::PopFont();

		}
		ImGui::EndChild();

		ImGui::GetWindowDrawList()->AddLine(ImGui::GetWindowPos() + ImVec2(0, logo_size_scale.y) - ImVec2(0, 2), ImGui::GetWindowPos() + logo_size_scale - ImVec2(0, 2), color);
		top_l = ImGui::GetWindowPos() + ImVec2(0, logo_size_scale.y);
		top_r = top_l + ImVec2(logo_size_scale.x, 0);
		a = 125.f;
		for (int i = 0; i < 10; i++) {
			auto color = ImGui::GetStyleColorVec4(ImGuiCol_BorderShadow);
			color.w = a / 255.f;
			ImGui::GetWindowDrawList()->AddLine(top_l, top_r, ImGui::ColorConvertFloat4ToU32(color));
			top_l.y += 1;
			top_r.y += 1;
			a -= 12.5f;
		}

		ImGui::ItemSize(ImVec2(0, 4 * menu_dpi_scale));

		ImGui::PushFont(GetDpiFont("main_small"));
		
		static int anim_alpha[7];
		static int anim_size[7];
		anim_alpha[cur_tab] = MenuAlpha.w * 255.f;

		if (ImGuiW::Button(U8ST("合法自瞄\n用合法的方式赢得胜利  "), ImVec2(tab_size_scale.x - ImGui::GetStyle().ItemSpacing.x * 2, 70 * menu_dpi_scale),0,cur_tab == 0,anim_alpha[0], anim_size[0])) cur_tab = 0;
		if (ImGuiW::Button(U8ST("暴力自瞄\n使用一切手段击败敌人  "), ImVec2(tab_size_scale.x - ImGui::GetStyle().ItemSpacing.x * 2, 70 * menu_dpi_scale), 0, cur_tab == 1, anim_alpha[1], anim_size[1])) cur_tab = 1;
		if (ImGuiW::Button(U8ST("超感视觉\n说人话就是esp         "), ImVec2(tab_size_scale.x - ImGui::GetStyle().ItemSpacing.x * 2, 70 * menu_dpi_scale), 0, cur_tab == 2, anim_alpha[2], anim_size[2])) cur_tab = 2;
		if (ImGuiW::Button(U8ST("其他设置\n有用的小功能          "), ImVec2(tab_size_scale.x - ImGui::GetStyle().ItemSpacing.x * 2, 70 * menu_dpi_scale), 0, cur_tab == 3, anim_alpha[3], anim_size[3])) cur_tab = 3;
		if (ImGuiW::Button(U8ST("用户面板\n登陆你的论坛账号！    "), ImVec2(tab_size_scale.x - ImGui::GetStyle().ItemSpacing.x * 2, 70 * menu_dpi_scale), 0, cur_tab == 4, anim_alpha[4], anim_size[4])) cur_tab = 4;

		ImGui::PopFont();

		ImGui::End();



		ImGui::Begin("SKYDOME_DEBUG", 0, /*ImGuiWindowFlags_NoResize |*/ ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

		make_header(U8ST("SKYDOME.PW - 调试"));




		ImGui::End();










	}





	
}

//或许，尝试在窗口过程中切换
void MenuManager::toggle(bool state)
{
	if (!ImGui::GetCurrentContext()) {
		return;
	}

	show_menu = state;
	
	
	if (show_menu)on_menu_open();
	if (/*g_interfaces->InputSystem->IsRelativeMouseMode()*/g_OffsetManager->fnGetRelativeMouseMode()) {
		const ImVec2 screenCenter = ImGui::GetIO().DisplaySize * 0.5f;

		//toggle_mouse = 200;

		g_OffsetManager->fnSetRelativeMouseMode(!show_menu);
		g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
		g_OffsetManager->fnWarpMouseInWindow(nullptr, screenCenter.x, screenCenter.y);

	}

}

#include <iostream>
#include <cstdlib> // for std::rand
#include <ctime>   // for std::time
#include <random> // C++11的随机数库

bool probabilityTrigger(double probability) {
	// 使用C++11引入的随机数引擎和分布
	static std::random_device rd; // 随机设备，用于获取种子
	static std::mt19937 gen(rd()); // Mersenne Twister随机数引擎
	std::uniform_real_distribution<> dis(0.0, 1.0); // 均匀分布在[0.0, 1.0)范围内

	double randNum = dis(gen); // 生成[0.0, 1.0)内的随机数

	return randNum < probability;
}


void MenuManager::make_header(std::string name, const char* font){

	ImGui::SetCursorPos(ImVec2(0, 0));
	if (ImGui::BeginChild("header", logo_size_scale, true, ImGuiWindowFlags_NoDecoration)) {
		ImGui::PushFont(GetDpiFont(font));
		ImVec2 text_size = ImGui::CalcTextSize(name.c_str());

		ImGui::SetCursorPos(ImVec2(12 * menu_dpi_scale, (logo_size_scale.y / 2) - (text_size.y / 2)));

		

		ImGui::Text(name.c_str());

		ImGui::PopFont();

	}ImGui::EndChild();

	//ImGui::GetWindowDrawList()->AddLine(ImGui::GetWindowPos() + ImVec2(0, logo_size_scale.y), ImGui::GetWindowPos() + logo_size_scale, color);

}

//在菜单出现时，启动淡出
void MenuManager::on_menu_open()
{

	std::srand(static_cast<unsigned>(std::time(nullptr))); // 初始化随机数种子

	if (pColorAnim == nullptr)
	{
		// Create the animation and attach it to the ImVec4 used to color the text
		pColorAnim = new imanim::ImVec4Anim(&vTextColor);
	}

	if (MenuAlphaAnim == nullptr)
	{
		// Create the animation and attach it to the ImVec4 used to color the text
		MenuAlphaAnim = new imanim::ImVec4Anim(&MenuAlpha);
	}

	if (probabilityTrigger(0.1)) {
		// Animate from white text to red text
		pColorAnim->setStartValue(ImVec4(1, 1, 1, 1));
		pColorAnim->setEndValue(ImVec4(1, 1, 1, 0));
		// Set the animation duration for a single loop (in seconds)
		pColorAnim->setDuration(4);
		// Set the number of loops of animation; -1 loops forever
		pColorAnim->setLoopCount(1);
		pColorAnim->setEasingCurve(imanim::EasingCurve::Type::OutInBounce);
		pColorAnim->start();

		logo_text_d = logo_texts[std::rand() % logo_texts.size()];
	}
	else {
		pColorAnim->stop();
		vTextColor = {1, 1, 1, 1};
		logo_text_d = logo_text;
	}
	
	MenuAlphaAnim->setStartValue(ImVec4(1, 1, 1, 0));
	MenuAlphaAnim->setEndValue(ImVec4(1, 1, 1, 1));
	MenuAlphaAnim->setDuration(0.3f);
	MenuAlphaAnim->setEasingCurve(imanim::EasingCurve::Type::InQuart);
	MenuAlphaAnim->start();
}

void MenuManager::on_anim_frame()
{
	if (pColorAnim->getStartValue().w == 1.f && 
		vTextColor.w == 0.f &&
		pColorAnim->getState() == imanim::AbstractAnimation::State::Stopped) {

		logo_text_d = logo_text;

		pColorAnim->setStartValue(ImVec4(1, 1, 1, 0));
		pColorAnim->setEndValue(ImVec4(1, 1, 1, 1));
		// Set the animation duration for a single loop (in seconds)
		pColorAnim->setDuration(1.0);
		// Set the number of loops of animation; -1 loops forever
		pColorAnim->setLoopCount(1);
		pColorAnim->setEasingCurve(imanim::EasingCurve::Type::OutQuad);

		pColorAnim->stop();
		pColorAnim->start();
		


	}
	

	pColorAnim->update();
	MenuAlphaAnim->update();
}
