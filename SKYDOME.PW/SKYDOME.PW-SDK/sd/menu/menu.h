#pragma once
#include "../interfaces/interfaces.h"


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
private:

};

inline MenuManager* g_MenuManager = new MenuManager;