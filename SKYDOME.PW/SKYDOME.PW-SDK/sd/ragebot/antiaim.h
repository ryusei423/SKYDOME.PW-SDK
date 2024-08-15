#pragma once
#include "../entitycache/entitycache.h"
#include "../menu/config.h"

class AntiAim {
public:
	void run(CUserCmd* cmd) {
		switch (GET_CFG("ragebot_pitch",it))
		{
		case 0:break;
		case 1:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.pitch = 89.f; break;
		case 2:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.pitch = -89.f; break;
		case 3:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.pitch = 0; break;
		case 4:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.pitch = MATH::fnRandomFloat(-89.F,89.F); break;

		default:
			break;
		}

		switch (GET_CFG("ragebot_yaw", it))
		{
		case 0:break;
		case 1:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.yaw += 180.f; break;
		case 2:cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.yaw = cmd->csgoUserCmd.pBaseCmd->nClientTick % 180; break;

		default:
			break;
		}

	
	};



};

inline AntiAim* g_AntiAim = new AntiAim;