#include "menu.h"

#include "../../external/imgui/imgui_freetype.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui_internal.h"

#include "../utilities/memory.h"
#include "../hooks/hooks.h"




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
	colors[ImGuiCol_ChildBg] = ImVec4(45.f / 255.f, 45.f / 255.f, 45.f / 255.f, 1.0f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.56f, 0.59f, 0.56f, 0.30f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(37.0f / 255.0f, 37.0f / 255.0f, 37.0f / 255.0f, 102.0f / 255.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(76.0f / 255.0f, 76.0f / 255.0f, 76.0f / 255.0f, 1.0f);
	colors[ImGuiCol_CheckMark] = ImVec4(132.0f / 255.0f, 132.0f / 255.0f, 132.0f / 255.0f, 1.0f);


	colors[ImGuiCol_Button] = ImVec4(0.56f, 0.59f, 0.56f, 0.30f);									//按钮颜色
	colors[ImGuiCol_ButtonHovered] = ImVec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);	//按钮在悬停时的颜色 我们的自定义控件将通过动画到达此颜色
	colors[ImGuiCol_ButtonActive] = ImVec4(76.0f / 255.0f, 76.0f / 255.0f, 76.0f / 255.0f, 1.0f);	//按钮被点击时的颜色

	colors[ImGuiCol_Header] = ImVec4(0.56f, 0.59f, 0.56f, 0.30f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.39f, 0.39f, 0.39f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

	//style->ChildBorderSize = 0;
}

#include "chinese.h"
#include "fonts/Roboto-Bold.h"
bool MenuManager::init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext){

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = io.LogFilename = nullptr;
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

	DefaultStyle = ImGui::GetStyle();

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

	fonts.insert(std::make_pair("main_font_75", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_075, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_font_small_75", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_075, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_font_75", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_075)));

	fonts.insert(std::make_pair("main_font_100", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_font_small_100", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_font_100", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f)));

	fonts.insert(std::make_pair("main_font_125", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_125, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_font_small_125", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_125, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_font_125", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_125)));

	fonts.insert(std::make_pair("main_font_150", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_150, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_font_small_150", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_150, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_font_150", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_150)));

	fonts.insert(std::make_pair("main_font_200", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 18.0f * menu_dpi_scale_200, NULL,/* io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("main_font_small_200", io.Fonts->AddFontFromFileTTF("C:/windows/fonts/simhei.ttf", 15.0f * menu_dpi_scale_200, NULL, /*io.Fonts->GetGlyphRangesChineseFull()*/myRange.Data)));
	fonts.insert(std::make_pair("logo_font_200", io.Fonts->AddFontFromMemoryCompressedTTF(Roboto_compressed_data, sizeof(Roboto_compressed_data), 17.f * menu_dpi_scale_200)));


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
		return fonts[font + XorStr("_200")];
	}

	return nullptr;

};

void MenuManager::frame(IDXGISwapChain* pSwapChain)
{
	/*if (g_OffsetManager->fnGetRelativeMouseMode()){
		g_OffsetManager->fnSetRelativeMouseMode(!show_menu);
		g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
	}*/
	

	/*if (toggle_mouse > 0) {
		toggle_mouse--;
		if (show_menu == false)
		{
			g_OffsetManager->fnSetRelativeMouseMode(!show_menu);
			g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
		}
		
	}*/
	//g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
	if (show_menu)
	{
		UpdateDpi();

		//ImGui::GetIO().MouseDrawCursor = true;
		ImVec2 main_windows_pos;

		ImGui::SetNextWindowSize(menu_size_scale);
		ImGui::Begin("SKYDOME",0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);

		main_windows_pos = ImGui::GetWindowPos();

		ImGui::SetCursorPos(ImVec2(0, 0));
		if (ImGui::BeginChild("top", logo_size_scale, true, ImGuiWindowFlags_NoDecoration)) {


			
			ImGui::PushFont(GetDpiFont("logo_font"));

			ImVec2 text_size = ImGui::CalcTextSize("SKYDOME.PW");

			ImGui::SetCursorPos(ImVec2(12 * menu_dpi_scale, (logo_size_scale.y / 2) - (text_size.y / 2)));
			ImGui::Text("SKYDOME.PW");
		
			ImGui::PopFont();

		}
		ImGui::EndChild();

		ImGui::PushFont(GetDpiFont("main_font"));

		const char* dpi_str[] = { "%75", "%100" , "%150" , "%175", "%200" };
		ImGui::Combo(U8ST("DPI缩放"), &dpi, dpi_str, IM_ARRAYSIZE(dpi_str));

		ImGui::PopFont();
		ImGui::End();

		ImGui::SetNextWindowSize(tab_size_scale);
		ImGui::SetNextWindowPos(main_windows_pos + ImVec2(menu_size_scale.x + 8 * menu_dpi_scale, 0));
		ImGui::Begin("SKYDOME_TAB", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
		ImGui::End();
	}




	
}

void MenuManager::toggle(bool state)
{
	if (!ImGui::GetCurrentContext()) {
		return;
	}

	show_menu = state;
	

	if (g_interfaces->InputSystem->IsRelativeMouseMode()/*g_OffsetManager->fnGetRelativeMouseMode()*/) {
		const ImVec2 screenCenter = ImGui::GetIO().DisplaySize * 0.5f;

		//toggle_mouse = 200;

		g_OffsetManager->fnSetRelativeMouseMode(!show_menu);
		g_OffsetManager->fnSetWindowMouseGrab(g_interfaces->InputSystem->GetSDLWindow(), !show_menu);
		g_OffsetManager->fnWarpMouseInWindow(nullptr, screenCenter.x, screenCenter.y);
	}

}
