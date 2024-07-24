#pragma once
// used: [crt] isfinite, fmodf, remainderf
#include <cmath>

#include "vector.h"

// used: clamp
#include "../../sd/utilities/crt.h"

// forward declarations
struct Matrix3x4_t;

struct QAngle
{
	constexpr QAngle(float pitch = 0.f, float yaw = 0.f, float roll = 0.f) :
		pitch(pitch), yaw(yaw), roll(roll) { }

	constexpr QAngle(const float* arrAngles) :
		pitch(arrAngles[0]), yaw(arrAngles[1]), roll(arrAngles[2]) { }

#pragma region qangle_array_operators

	[[nodiscard]] float& operator[](const int nIndex)
	{
		return reinterpret_cast<float*>(this)[nIndex];
	}

	[[nodiscard]] const float& operator[](const int nIndex) const
	{
		return reinterpret_cast<const float*>(this)[nIndex];
	}

#pragma endregion

#pragma region qangle_relational_operators

	bool operator==(const QAngle& angBase) const
	{
		return this->IsEqual(angBase);
	}

	bool operator!=(const QAngle& angBase) const
	{
		return !this->IsEqual(angBase);
	}

#pragma endregion

#pragma region qangle_assignment_operators

	constexpr QAngle& operator=(const QAngle& angBase)
	{
		this->pitch = angBase.pitch;
		this->yaw = angBase.yaw;
		this->roll = angBase.roll;
		return *this;
	}

#pragma endregion

#pragma region qangle_arithmetic_assignment_operators

	constexpr QAngle& operator+=(const QAngle& angBase)
	{
		this->pitch += angBase.pitch;
		this->yaw += angBase.yaw;
		this->roll += angBase.roll;
		return *this;
	}

	constexpr QAngle& operator-=(const QAngle& angBase)
	{
		this->pitch -= angBase.pitch;
		this->yaw -= angBase.yaw;
		this->roll -= angBase.roll;
		return *this;
	}

	constexpr QAngle& operator*=(const QAngle& angBase)
	{
		this->pitch *= angBase.pitch;
		this->yaw *= angBase.yaw;
		this->roll *= angBase.roll;
		return *this;
	}

	constexpr QAngle& operator/=(const QAngle& angBase)
	{
		this->pitch /= angBase.pitch;
		this->yaw /= angBase.yaw;
		this->roll /= angBase.roll;
		return *this;
	}

	constexpr QAngle& operator+=(const float flAdd)
	{
		this->pitch += flAdd;
		this->yaw += flAdd;
		this->roll += flAdd;
		return *this;
	}

	constexpr QAngle& operator-=(const float flSubtract)
	{
		this->pitch -= flSubtract;
		this->yaw -= flSubtract;
		this->roll -= flSubtract;
		return *this;
	}

	constexpr QAngle& operator*=(const float flMultiply)
	{
		this->pitch *= flMultiply;
		this->yaw *= flMultiply;
		this->roll *= flMultiply;
		return *this;
	}

	constexpr QAngle& operator/=(const float flDivide)
	{
		this->pitch /= flDivide;
		this->yaw /= flDivide;
		this->roll /= flDivide;
		return *this;
	}

#pragma endregion

#pragma region qangle_arithmetic_unary_operators

	constexpr QAngle& operator-()
	{
		this->pitch = -this->pitch;
		this->yaw = -this->yaw;
		this->roll = -this->roll;
		return *this;
	}

	constexpr QAngle operator-() const
	{
		return { -this->pitch, -this->yaw, -this->roll };
	}

#pragma endregion

#pragma region qangle_arithmetic_ternary_operators

	constexpr QAngle operator+(const QAngle& angAdd) const
	{
		return { this->pitch + angAdd.pitch, this->yaw + angAdd.yaw, this->roll + angAdd.roll };
	}

	constexpr QAngle operator-(const QAngle& angSubtract) const
	{
		return { this->pitch - angSubtract.pitch, this->yaw - angSubtract.yaw, this->roll - angSubtract.roll };
	}

	constexpr QAngle operator*(const QAngle& angMultiply) const
	{
		return { this->pitch * angMultiply.pitch, this->yaw * angMultiply.yaw, this->roll * angMultiply.roll };
	}

	constexpr QAngle operator/(const QAngle& angDivide) const
	{
		return { this->pitch / angDivide.pitch, this->yaw / angDivide.yaw, this->roll / angDivide.roll };
	}

	constexpr QAngle operator+(const float flAdd) const
	{
		return { this->pitch + flAdd, this->yaw + flAdd, this->roll + flAdd };
	}

	constexpr QAngle operator-(const float flSubtract) const
	{
		return { this->pitch - flSubtract, this->yaw - flSubtract, this->roll - flSubtract };
	}

	constexpr QAngle operator*(const float flMultiply) const
	{
		return { this->pitch * flMultiply, this->yaw * flMultiply, this->roll * flMultiply };
	}

	constexpr QAngle operator/(const float flDivide) const
	{
		return { this->pitch / flDivide, this->yaw / flDivide, this->roll / flDivide };
	}

#pragma endregion

	// @returns : true if each component of angle is finite, false otherwise
	[[nodiscard]] bool IsValid() const
	{
		return (std::isfinite(this->pitch) && std::isfinite(this->yaw) && std::isfinite(this->roll));
	}

	/// @returns: true if each component of angle equals to another, false otherwise
	[[nodiscard]] bool IsEqual(const QAngle& angEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const
	{
		return (std::fabsf(this->pitch - angEqual.pitch) < flErrorMargin && std::fabsf(this->yaw - angEqual.yaw) < flErrorMargin && std::fabsf(this->roll - angEqual.roll) < flErrorMargin);
	}

	/// @returns: true if each component of angle equals zero, false otherwise
	[[nodiscard]] bool IsZero() const
	{
		// @test: to make this implementation right, we should use fpclassify here, but game aren't doing same, probably it's better to keep this same, just ensure that it will be compiled same
		return (this->pitch == 0.0f && this->yaw == 0.0f && this->roll == 0.0f);
	}

	/// @returns: length of hypotenuse
	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(pitch * pitch + yaw * yaw);
	}

	/// clamp each angle component by minimal/maximal allowed value for source sdk games
	/// @returns: clamped angle
	constexpr QAngle& Clamp()
	{
		this->pitch = MATH::Clamp(this->pitch, -89.f, 89.f);
		this->yaw = MATH::Clamp(this->yaw, -180.f, 180.f);
		this->roll = MATH::Clamp(this->roll, -45.f, 45.f);
		return *this;
	}

	/// map polar angles to the range of [-180, 180] degrees
	/// @returns: normalized angle
	QAngle& Normalize()
	{
		this->pitch = std::remainderf(this->pitch, 360.f);
		this->yaw = std::remainderf(this->yaw, 360.f);
		this->roll = std::remainderf(this->roll, 360.f);
		return *this;
	}

	/// convert angle to direction vectors
	/// @param[out] pvecForward [optional] output for converted forward vector
	/// @param[out] pvecRight [optional] output for converted right vector
	/// @param[out] pvecUp [optional] output for converted up vector
	void ToDirections(Vector* pvecForward, Vector* pvecRight = nullptr, Vector* pvecUp = nullptr) const;

	/// @param[in] vecOrigin [optional] origin for converted matrix
	/// @returns: matrix converted from angle
	[[nodiscard]] Matrix3x4_t ToMatrix(const Vector& vecOrigin = {}) const;

public:
	float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
};