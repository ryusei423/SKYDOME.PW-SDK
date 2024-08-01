#pragma once

#include "../entitycache/entitycache.h"

/*
	FSN:
		保存tick hitbox bone
		或许可以在fsn中计算所有点?这样可以省去保存hitbox,并且我想能加速射击

	CREATEMOVE
		如果在FSN中计算了点，那就轻松多了
		毕竟射击的目标不是以时间来分割的,而是一个又一个的记录

*/
struct RageBotTarget{
	CachedEntity player;


};

struct RageBotPoint {
	CachedEntity player;
	Vector point;
	int tick;

};

class RageBot {
public:

	void run(CUserCmd* cmd);
	void DumpCmdInfo(CUserCmd* cmd);
	void RemoveRecoil();
	bool FindTarget();
	bool CanRunRage();
	std::vector<RageBotTarget> noobs;
	std::vector<RageBotPoint> points;
private:
	void SetCmdTick(int tick, C_CSPlayerPawn* target,int history = 0);
	QAngle ang_;
	CUserCmd* cur_cmd;
};



inline RageBot* g_RageBot = new RageBot;