#include"movement.h"


struct MovementData {
	QAngle angCorrectionView{};

} _move_data;

enum DIR : int {
	FORWARDS = 0,
	BACKWARDS = 180,
	LEFT = 90,
	RIGHT = -90
};

enum MODE : int {
	legit = 0,
	rage = 1
};

void MovementManager::AngleQangles(const QAngle& angles, QAngle* forward, QAngle* right, QAngle* up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	// Convert angles from degrees to radians
	angle = angles.yaw * (MATH::_PI / 180.0);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles.pitch * (MATH::_PI / 180.0);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles.roll * (MATH::_PI / 180.0);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->pitch = cp * cy;
		forward->yaw = cp * sy;
		forward->roll = -sp;
	}

	if (right)
	{
		right->pitch = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->yaw = (-1 * sr * sp * sy + -1 * cr * cy);
		right->roll = -1 * sr * cp;
	}

	if (up)
	{
		up->pitch = (cr * sp * cy + -sr * -sy);
		up->yaw = (cr * sp * sy + -sr * cy);
		up->roll = cr * cp;
	}
}


void MovementManager::InitTick(CUserCmd* cmd){
	save_angle = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
}

void MovementManager::EndTick(CUserCmd* cmd){
	auto usercmd = cmd->csgoUserCmd.pBaseCmd;
	movment_fix(cmd,save_angle);

	// correct movement buttons while player move have different to buttons values
	// clear all of the move buttons states
	cmd->nButtons.nValue &= (~IN_FORWARD | ~IN_BACK | ~IN_LEFT | ~IN_RIGHT);

	// re-store buttons by active forward/side moves
	if (usercmd->flForwardMove > 0.0f)
		cmd->nButtons.nValue |= IN_FORWARD;
	else if (usercmd->flForwardMove < 0.0f)
		cmd->nButtons.nValue |= IN_BACK;

	if (usercmd->flSideMove > 0.0f)
		cmd->nButtons.nValue |= IN_RIGHT;
	else if (usercmd->flSideMove < 0.0f)
		cmd->nButtons.nValue |= IN_LEFT;

	cmd->csgoUserCmd.pBaseCmd->CheckAndSetBits(EBaseCmdBits::BASE_BITS_FORWARDMOVE);
	cmd->csgoUserCmd.pBaseCmd->CheckAndSetBits(EBaseCmdBits::BASE_BITS_LEFTMOVE);
	cmd->csgoUserCmd.pBaseCmd->CheckAndSetBits(EBaseCmdBits::BASE_BITS_UPMOVE);

}

void MovementManager::movment_fix(CUserCmd* pCmd, QAngle angle)
{
	QAngle wish_angle;
	wish_angle = /*pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue*/angle;
	int revers = wish_angle.pitch > 89.f ? -1 : 1;
	wish_angle.Clamp();

	QAngle view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = /*angle*/pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;

	AngleQangles(wish_angle, &view_fwd, &view_right, &view_up);
	AngleQangles(viewangles, &cmd_fwd, &cmd_right, &cmd_up);

	const float v8 = sqrtf((view_fwd.pitch * view_fwd.pitch) + (view_fwd.yaw * view_fwd.yaw));
	const float v10 = sqrtf((view_right.pitch * view_right.pitch) + (view_right.yaw * view_right.yaw));
	const float v12 = sqrtf(view_up.roll * view_up.roll);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.pitch, (1.f / v8) * view_fwd.yaw, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.pitch, (1.f / v10) * view_right.yaw, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.roll);

	const float v14 = sqrtf((cmd_fwd.pitch * cmd_fwd.pitch) + (cmd_fwd.yaw * cmd_fwd.yaw));
	const float v16 = sqrtf((cmd_right.pitch * cmd_right.pitch) + (cmd_right.yaw * cmd_right.yaw));
	const float v18 = sqrtf(cmd_up.roll * cmd_up.roll);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.pitch, (1.f / v14) * cmd_fwd.yaw, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.pitch, (1.f / v16) * cmd_right.yaw, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.roll);

	const float v22 = norm_view_fwd.x * pCmd->csgoUserCmd.pBaseCmd->flForwardMove;
	const float v26 = norm_view_fwd.y * pCmd->csgoUserCmd.pBaseCmd->flForwardMove;
	const float v28 = norm_view_fwd.z * pCmd->csgoUserCmd.pBaseCmd->flForwardMove;
	const float v24 = norm_view_right.x * pCmd->csgoUserCmd.pBaseCmd->flSideMove;
	const float v23 = norm_view_right.y * pCmd->csgoUserCmd.pBaseCmd->flSideMove;
	const float v25 = norm_view_right.z * pCmd->csgoUserCmd.pBaseCmd->flSideMove;
	const float v30 = norm_view_up.x * pCmd->csgoUserCmd.pBaseCmd->flUpMove;
	const float v27 = norm_view_up.z * pCmd->csgoUserCmd.pBaseCmd->flUpMove;
	const float v29 = norm_view_up.y * pCmd->csgoUserCmd.pBaseCmd->flUpMove;

	pCmd->csgoUserCmd.pBaseCmd->flForwardMove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	pCmd->csgoUserCmd.pBaseCmd->flSideMove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	pCmd->csgoUserCmd.pBaseCmd->flUpMove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	pCmd->csgoUserCmd.pBaseCmd->flForwardMove = revers * ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25)) + (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)));

	pCmd->csgoUserCmd.pBaseCmd->flSideMove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25)) + (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)));

	pCmd->csgoUserCmd.pBaseCmd->flForwardMove = std::clamp(pCmd->csgoUserCmd.pBaseCmd->flForwardMove, -1.f, 1.f);

	pCmd->csgoUserCmd.pBaseCmd->flSideMove = std::clamp(pCmd->csgoUserCmd.pBaseCmd->flSideMove, -1.f, 1.f);
}