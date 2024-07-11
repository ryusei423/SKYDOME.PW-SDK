#pragma once
#include "../interfaces/interfaces.h"


class MenuManager
{
public:
	bool init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void frame();
	void on_resizebuffers();
	void on_createswapchain();
	void toggle(bool state);
	bool show_menu = false;
private:

};

inline MenuManager* g_MenuManager = new MenuManager;