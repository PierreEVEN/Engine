#pragma once
#include "Vector.h"
#include "Maths/BaseOperations.h"
#include <valarray>
#include "Types\Rotator.refl.h" // automatically generated reflection header

REFLECT(template = { {float, SQuatf}, {double, SQuatd} })
template<typename T>
struct IQuaternion final
{
	REFLECT_BODY()
		inline IQuaternion() : x(0), y(0), z(0), w(0) {}
	inline IQuaternion(const T& inX, const T& inY, const T& inZ, const T& inW) : x(inX), y(inY), z(inZ), w(inW) {}
	inline IQuaternion(const T& inRoll, const T& inPitch, const T& inYaw) { SetFromEuleurAngles(-inRoll + 180, -inPitch, inYaw); }
	IQuaternion(SVector inForwardVector, T inAngle) { SetFromForwardVectorAndAngle(inForwardVector.x, inForwardVector.y, inForwardVector.z, inAngle); }

	/* [] operator */
	inline T& operator[](const size_t& axis) {
		return coords[axis];
	}

	inline const T& operator[](const size_t& axis) const {
		return coords[axis];
	}

	inline operator String() const {
		return ToString(x) + ", " + ToString(y) + ", " + ToString(z) + ", " + ToString(w);
	}

	inline const SVector GetForwardVector() const {
		return SVector(
			1 - 2 * (y * y + z * z),
			2 * (x * y + w * z),
			2 * (x * z - w * y)
		);
	}

	inline const SVector GetRightVector() const {
		return SVector(
			2 * (x * y - w * z),
			1 - 2 * (x * x + z * z),
			2 * (y * z + w * x)
		);
	}

	inline const SVector GetUpVector() const {
		return SVector(
			2 * (x * z + w * y),
			2 * (y * z - w * x),
			1 - 2 * (x * x + y * y)
		);
	}

	inline const T Roll() const {
		T sinr_cosp = 2 * (w * x + y * z);
		T cosr_cosp = 1 - 2 * (x * x + y * y);
		return std::atan2(sinr_cosp, cosr_cosp) / G_FPI * 180;
	}

	inline const T Pitch() const {
		T sinp = 2 * (w * y - z * x);
		if (std::abs(sinp) >= 1)
			return std::copysign(G_FPI / 2, sinp) / G_FPI * 180; // use 90 degrees if out of range
		else
			return std::asin(sinp) / G_FPI * 180;
	}

	inline const T Yaw() const {
		T siny_cosp = 2 * (w * z + x * y);
		T cosy_cosp = 1 - 2 * (y * y + z * z);
		return std::atan2(siny_cosp, cosy_cosp) / G_FPI * 180;
	}

	union
	{
		struct { T x, y, z, w; };
		T coords[4];
	};


private:

	inline void SetFromEuleurAngles(const T& inRoll, const T& inPitch, const T& inYaw)
	{
		T cy = cos(((inYaw / 180) * G_FPI) * static_cast<T>(0.5));
		T sy = sin(((inYaw / 180) * G_FPI) * static_cast<T>(0.5));
		T cp = cos(((inPitch / 180) * G_FPI) * static_cast<T>(0.5));
		T sp = sin(((inPitch / 180) * G_FPI) * static_cast<T>(0.5));
		T cr = cos(((inRoll / 180) * G_FPI) * static_cast<T>(0.5));
		T sr = sin(((inRoll / 180) * G_FPI) * static_cast<T>(0.5));

		w = cr * cp * cy + sr * sp * sy;
		x = sr * cp * cy - cr * sp * sy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
	}

	inline void SetFromForwardVectorAndAngle(const T& inX, const T& inY, const T& inZ, const T& inAngle)
	{
		T rAngle = (inAngle / 180) * G_FPI;
		T s = sin(rAngle / 2);
		x = (inX + 180) * s;
		y = inY * s;
		z = inZ * s;
		w = cos(rAngle / 2);
	}
};

REFLECT(template = { {float, SRotatorf}, {double, SRotatord} })
template<typename T>
struct IRotator final
{
	REFLECT_BODY()
		inline IRotator() : x(0), y(0), z(0) {}
	inline IRotator(const T& inRoll, const T& inPitch, const T& inYaw) : roll(inRoll), pitch(inPitch), yaw(inYaw) {}
	inline IRotator(const IQuaternion<T>& inQuat) : roll(inQuat.Roll()), pitch(inQuat.Pitch()), yaw(inQuat.Yaw()) {}

	inline operator IQuaternion<T>() {
		return IQuaternion<T>(roll, pitch, yaw);
	}

	inline operator String() const {
		return ToString(x) + ", " + ToString(y) + ", " + ToString(z);
	}

	union
	{
		struct { T roll, pitch, yaw; };
		struct { T x, y, z; };
		T coords[3];
	};
};
