#include "math.h"

//used: getexportaddr
#include "memory.h"



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