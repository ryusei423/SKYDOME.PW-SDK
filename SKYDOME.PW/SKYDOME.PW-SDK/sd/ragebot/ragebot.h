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
	bool FindTarget();
	std::vector<RageBotTarget> noobs;
	std::vector<RageBotPoint> points;
};



inline RageBot* g_RageBot = new RageBot;