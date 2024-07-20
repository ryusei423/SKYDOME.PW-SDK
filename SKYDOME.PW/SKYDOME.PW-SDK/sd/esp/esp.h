#pragma once

//��CSGOʱ����������Ϊ"esp"����ƺ���Щ�����������������ߵ�������
//Ȼ���ҽ�����������ʱ�����һ�£�Extra-Sensory Perception�����й�֪����
//�е�ᣬ�Ҵ�������ʹ���������(��realy 19c-aim v0.15���ʼ����������esp���ں��ڵĸ�������дʱ������Ϊplayervision)
#include"../../sdk/datatypes/matrix.h"
#include "../../external/imgui/imgui.h"



struct EspItemElement
{
	int pos;
	int type;
	ImU32 color;
	std::string text;
	bool show_text;
	float bar;

};

//����ÿһ֡���´�����
struct EspItem{
	Vector pos;
	ImVec2 size;
	bool outline;
	std::vector<EspItemElement> Elements;
};

class EspDrawManager
{
public:
	void DrawFrame(ImDrawList* drawlist);
	void MakeFrame();
	bool WorldToScreen(const Vector& vecOrigin, ImVec2* pvecScreen);
	ViewMatrix_t matrix;
	std::vector<EspItem> manydogs;
private:
	



};

inline EspDrawManager* g_EspDrawManager = new EspDrawManager;