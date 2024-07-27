#include "../utilities/math.h"
#include "../../sdk/datatypes/qangle.h"
#include "../../sdk/datatypes/usercmd.h"


class MovementManager
{
public:
	void InitTick(CUserCmd* cmd);

	void EndTick(CUserCmd* cmd);
private:
	void movment_fix(CUserCmd* pCmd, QAngle angle);
	void AngleQangles(const QAngle& angles, QAngle* forward, QAngle* right, QAngle* up);
	QAngle save_angle;
};

inline MovementManager* g_MovementManager = new MovementManager;

