#pragma once

#include "../entitycache/entitycache.h"

/*
	FSN:
		����tick hitbox bone
		���������fsn�м������е�?��������ʡȥ����hitbox,���������ܼ������

	CREATEMOVE
		�����FSN�м����˵㣬�Ǿ����ɶ���
		�Ͼ������Ŀ�겻����ʱ�����ָ��,����һ����һ���ļ�¼

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