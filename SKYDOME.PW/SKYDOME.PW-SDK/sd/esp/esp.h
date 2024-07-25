#pragma once

//��CSGOʱ����������Ϊ"esp"����ƺ���Щ�����������������ߵ�������
//Ȼ���ҽ�����������ʱ�����һ�£�Extra-Sensory Perception�����й�֪����
//�е�ᣬ�Ҵ�������ʹ���������(��realy 19c-aim v0.15���ʼ����������esp���ں��ڵĸ�������дʱ������Ϊplayervision)
#include"../../sdk/datatypes/matrix.h"
#include "../../external/imgui/imgui.h"



struct EspItemElement
{
	enum 
	{
		TOP, 
		LEFT, 
		RIGHT, 
		BOTTOM,
		TEXT,
		BAR

	};


	int pos;
	int type;
	ImU32 color = IM_COL32_WHITE;
	std::string text;
	bool show_text;
	float bar;
	
};

//����ÿһ֡���´�����
struct EspItem{
	
	Vector pos;
	Vector head;
	ImVec4 box;
	bool is_visible;
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
	
	bool GetEntityBoundingBox(void* pEntity, ImVec4* pVecOut);


};

inline EspDrawManager* g_EspDrawManager = new EspDrawManager;