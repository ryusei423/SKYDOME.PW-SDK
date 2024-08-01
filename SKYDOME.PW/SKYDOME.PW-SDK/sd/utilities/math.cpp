#include "math.h"

//used: getexportaddr
#include "memory.h"

#include "../../sdk/datatypes/qangle.h"

bool MATH::Setup()
{
	bool bSuccess = true;

	const void* hTier0Lib = MEM::GetModuleBaseHandle(TIER0_DLL);
	if (hTier0Lib == nullptr)
		return false;

	fnRandomSeed = reinterpret_cast<decltype(fnRandomSeed)>(MEM::GetExportAddress(hTier0Lib, XorStr("RandomSeed")));
	bSuccess &= (fnRandomSeed != nullptr);

	fnRandomFloat = reinterpret_cast<decltype(fnRandomFloat)>(MEM::GetExportAddress(hTier0Lib, XorStr("RandomFloat")));
	bSuccess &= (fnRandomFloat != nullptr);

	fnRandomFloatExp = reinterpret_cast<decltype(fnRandomFloatExp)>(MEM::GetExportAddress(hTier0Lib, XorStr("RandomFloatExp")));
	bSuccess &= (fnRandomFloatExp != nullptr);

	fnRandomInt = reinterpret_cast<decltype(fnRandomInt)>(MEM::GetExportAddress(hTier0Lib, XorStr("RandomInt")));
	bSuccess &= (fnRandomInt != nullptr);

	fnRandomGaussianFloat = reinterpret_cast<decltype(fnRandomGaussianFloat)>(MEM::GetExportAddress(hTier0Lib, XorStr("RandomGaussianFloat")));
	bSuccess &= (fnRandomGaussianFloat != nullptr);

	return bSuccess;
}

void MATH::TransformAABB(const Matrix3x4_t& transform, const Vector& minsIn, const Vector& maxsIn, Vector& minsOut, Vector& maxsOut)
{

	const Vector localCenter = (minsIn + maxsIn) * 0.5f;
	const Vector localExtent = maxsIn - localCenter;

	const auto& mat = transform.arrData;
	const Vector worldAxisX{ mat[0][0], mat[0][1], mat[0][2] };
	const Vector worldAxisY{ mat[1][0], mat[1][1], mat[1][2] };
	const Vector worldAxisZ{ mat[2][0], mat[2][1], mat[2][2] };

	const Vector worldCenter = localCenter.Transform(transform);
	const Vector worldExtent{
		localExtent.DotProductAbsolute(worldAxisX),
		localExtent.DotProductAbsolute(worldAxisY),
		localExtent.DotProductAbsolute(worldAxisZ),
	};

	minsOut = worldCenter - worldExtent;
	maxsOut = worldCenter + worldExtent;

}

void MATH::AngleVectors(const QAngle& angles, Vector* forward, Vector* right, Vector* up)
{
	float cp = std::cos(M_DEG2RAD(angles.pitch)), sp = std::sin(M_DEG2RAD(angles.pitch));
	float cy = std::cos(M_DEG2RAD(angles.yaw)), sy = std::sin(M_DEG2RAD(angles.yaw));
	float cr = std::cos(M_DEG2RAD(angles.roll)), sr = std::sin(M_DEG2RAD(angles.roll));

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
		right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
		right->z = -1.f * sr * cp;
	}

	if (up) {
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}


void MATH::AngleVectors(const QAngle& angles, Vector* forward)
{


	float sp, sy, cp, cy;

	sy = sin(M_DEG2RAD(angles[1]));
	cy = cos(M_DEG2RAD(angles[1]));

	sp = sin(M_DEG2RAD(angles[0]));
	cp = cos(M_DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void MATH::VectorAngles(Vector forward, QAngle* angles){

	float tmp, yaw, pitch;

	if (forward.y == 0.f && forward.x == 0.f) {
		yaw = 0;
		if (forward.z > 0) {
			pitch = 270;
		}
		else {
			pitch = 90.f;
		}
	}
	else {
		yaw = (float)(atan2(forward.y, forward.x) * 180.f / 3.14159265358979323846f);
		if (yaw < 0) {
			yaw += 360.f;
		}
		tmp = (float)sqrt(forward.x * forward.x + forward.y * forward.y);
		pitch = (float)(atan2(-forward.z, tmp) * 180.f / 3.14159265358979323846f);
		if (pitch < 0) {
			pitch += 360.f;
		}
	}
	angles->pitch = pitch;
	angles->yaw = yaw;
	angles->roll = 0.f;


}
