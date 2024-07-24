#pragma once

#include "../entitycache/entitycache.h"

struct LegitBotTarget {
	CachedEntity player;


};


class LegitBot {
public:

	void run(CUserCmd* cmd);
	bool FindTarget();
	std::vector<LegitBotTarget> noobs;

private:
	void TriggerBot();

	CUserCmd* cur_cmd;
	C_CSWeaponBase* weapon;
};