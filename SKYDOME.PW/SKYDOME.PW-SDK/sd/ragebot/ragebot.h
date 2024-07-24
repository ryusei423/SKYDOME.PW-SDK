#pragma once

#include "../entitycache/entitycache.h"

struct RageBotTarget{
	CachedEntity player;


};

struct RageBotPoint {
	Vector point;


};

class RageBot {
public:

	void run(CUserCmd* cmd);
	void RemoveRecoil();
	bool FindTarget();
	std::vector<RageBotTarget> noobs;
	std::vector<RageBotPoint> points;
private:
	QAngle ang_;
	CUserCmd* cur_cmd;
};



inline RageBot* g_RageBot = new RageBot;