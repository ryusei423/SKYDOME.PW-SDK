#pragma once

//在CSGO时代我曾经认为"esp"这个称呼有些土，就像我们作弊者的土话？
//然后我今天在命名的时候查了一下，Extra-Sensory Perception，超感官知觉？
//有点酷，我打算重新使用这个命名(在realy 19c-aim v0.15中最开始的命名就是esp，在后期的更新中重写时被命名为playervision)
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

//会在每一帧重新创建，
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