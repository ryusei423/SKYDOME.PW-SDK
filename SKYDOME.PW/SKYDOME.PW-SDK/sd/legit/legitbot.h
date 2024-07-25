#pragma once

#include "../entitycache/entitycache.h"

struct LegitBotTarget {
	CachedEntity player;
};

//����ֻ����������ĵ�
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