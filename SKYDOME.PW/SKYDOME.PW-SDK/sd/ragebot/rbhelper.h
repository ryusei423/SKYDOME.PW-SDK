#pragma once
#include "ragebot.h"

class RageBotHelper
{
public:
	//����ԭ��
	//ֻ��Ϊ�˾���ʵʩ���ݵĸ�����֤
	//��Щ����Ҫ�Ľ�
	void AutomaticStop(C_CSPlayerPawn* pLocal, C_CSWeaponBase* weapon, CUserCmd* cmd);
	bool Hitchance(C_CSPlayerPawn* pLocal, C_CSPlayerPawn* record, C_CSWeaponBase* weapon, QAngle vAimpoint, float hc);
private:







};



inline RageBotHelper* g_RageBotHelper = new RageBotHelper;