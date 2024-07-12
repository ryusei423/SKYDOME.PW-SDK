#pragma once
#include "../interfaces/interfaces.h"


class MenuManager
{
public:
	bool init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);//���ò˵���һЩ������Ϣ
	bool create(HWND hWnd);//�����豸�ͽ�����
	void CreateRenderTarget(IDXGISwapChain* pSwapChain);//������ȾĿ��
	void frame(IDXGISwapChain* pSwapChain);//������⴦��ֻ����ñ����ͽ���
	void toggle(bool state);

	bool show_menu = false;
	int toggle_mouse = 0;
private:

};

inline MenuManager* g_MenuManager = new MenuManager;