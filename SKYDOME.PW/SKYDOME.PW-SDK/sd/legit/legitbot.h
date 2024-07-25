#pragma once

#include "../entitycache/entitycache.h"

struct LegitBotTarget {
	CachedEntity player;
};

//我们只储存骨骼中心点
struct LegitBotPoint {
	Vector point;

};


class LegitBot {
public:

	void run(CUserCmd* cmd);
	bool FindTarget();
	

private:
	void TriggerBot();
	void Scan();

	std::vector<LegitBotTarget> noobs;
	std::vector<LegitBotPoint> points;

	CUserCmd* cur_cmd;
	C_CSWeaponBase* weapon;
};