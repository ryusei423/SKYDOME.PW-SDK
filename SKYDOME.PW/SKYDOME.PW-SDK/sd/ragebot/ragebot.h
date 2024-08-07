#pragma once

#include "../entitycache/entitycache.h"
#include "playerlog.h"
/*
	FSN:
		保存tick hitbox bone
		或许可以在fsn中计算所有点?这样可以省去保存hitbox,并且我想能加速射击

	CREATEMOVE
		如果在FSN中计算了点，那就轻松多了
		毕竟射击的目标不是以时间来分割的,而是一个又一个的记录

*/

//测试将会在预测eyepos上运行

struct RageBotPoint {
	CachedEntity player;
	Vector point;
	int tick;
	bool hcok;
	int health;
	int damage;

};


struct RageBotTarget{
	CachedEntity player;
	std::vector<lag_record_t>records;
	std::vector<lag_record_t>records_for_test;

	std::vector<RageBotPoint> player_points;
	std::vector<RageBotPoint> player_points_pred;

	RageBotPoint player_best_point;

	bool pred_pt2save_pt();
	bool set_best_point();
};



class RageBot {
public:
	enum 
	{
		AWTEST_PRED,
		AWTEST_SAVE,

		TS_ALL_OK,
		TS_FIRST_OK,
		TS_HALF_OK,
		TS_BACK_OK,
		TS_ALL_FAIL
	};

	void run(CUserCmd* cmd);
	void DumpCmdInfo(CUserCmd* cmd);
	void RemoveRecoil();
	bool FindTarget();
	bool CanRunRage();

	int RunTest(RageBotTarget* target);
	void AutoWallTest(Vector point, CachedEntity target,bool& canhit,float& damage,int test_type);

	bool MainScan();
	int RecordScan(lag_record_t* record, RageBotTarget* target,bool is_test = false);

	bool TestPointToSavePoint(RageBotPoint pt);
	bool FindBestPoint();
	void Fire();
	std::vector<RageBotTarget> noobs;
	std::vector<RageBotPoint> points;

private:
	RageBotPoint best_point;
	bool best_point_finded;
	bool hc_ok;
	bool have_test_ok;
	void SetCmdTick(int tick, C_CSPlayerPawn* target,int history = 0);
	QAngle ang_;
	CUserCmd* cur_cmd;
};



inline RageBot* g_RageBot = new RageBot;