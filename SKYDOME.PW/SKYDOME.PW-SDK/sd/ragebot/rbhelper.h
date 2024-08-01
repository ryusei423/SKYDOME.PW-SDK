#pragma once
#include "ragebot.h"

class RageBotHelper
{
public:
	//来自原子
	//只是为了尽早实施回溯的概念验证
	//这些都需要改进
	void AutomaticStop(C_CSPlayerPawn* pLocal, C_CSWeaponBase* weapon, CUserCmd* cmd);
	bool Hitchance(C_CSPlayerPawn* pLocal, C_CSPlayerPawn* record, C_CSWeaponBase* weapon, QAngle vAimpoint, float hc);
private:







};



inline RageBotHelper* g_RageBotHelper = new RageBotHelper;