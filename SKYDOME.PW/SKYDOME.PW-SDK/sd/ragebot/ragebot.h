#pragma once

#include "../entitycache/entitycache.h"

struct RageBotTarget{
	CachedEntity player;


};

struct RageBotPoint {
	Vector point;
	int tick;

};

class RageBot {
public:

	void run(CUserCmd* cmd);
	void RemoveRecoil();
	bool FindTarget();
	bool CanRunRage();
	std::vector<RageBotTarget> noobs;
	std::vector<RageBotPoint> points;
private:
	void SetCmdTick(int tick,int history = 0);
	QAngle ang_;
	CUserCmd* cur_cmd;
};



inline RageBot* g_RageBot = new RageBot;