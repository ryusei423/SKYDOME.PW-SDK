#include "../utilities/math.h"
#include "../../sdk/datatypes/qangle.h"
#include "../../sdk/datatypes/usercmd.h"


class MovementManager
{
public:
	void InitTick(CUserCmd* cmd);

	void EndTick(CUserCmd* cmd);
	QAngle save_angle;
private:
	void movment_fix(CUserCmd* pCmd, QAngle angle);
	void AngleQangles(const QAngle& angles, QAngle* forward, QAngle* right, QAngle* up);

};

inline MovementManager* g_MovementManager = new MovementManager;

