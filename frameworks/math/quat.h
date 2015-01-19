#pragma once

#include "vec3.h"
#include "mat4.h"
#include <algorithm>
#define QUAT_PI 3.1415926

class Quat
{
public:
	float x, y, z, w;

public:
	Quat() :w(1), x(0), y(0), z(0) {};
	Quat(float ww, float xx, float yy, float zz);
	~Quat() {};

	// add operator
	inline Quat operator+(const Quat& b) const;
	inline Quat operator* (float fScalar) const;
	inline Quat operator*=(float s);
	inline Quat operator*(const Quat& b) const;


	float dot(const Quat& q) const;
	Mat4  toMatrix() const;
	void  fromAxisAngle(const Vec3& axis, float angle);
	Quat& slerp(Quat q1, Quat q2, float time, float threshold = 1.0);
	Quat& lerp(Quat q1, Quat q2, float time);

	inline void toEuler(Vec3& euler) const;
};

inline Quat Quat::operator+(const Quat& b) const
{
	return Quat(w + b.w, x + b.x, y + b.y, z + b.z);
}

inline Quat Quat::operator* (float fscalar) const
{
	return Quat(fscalar*w, fscalar*x, fscalar*y, fscalar*z);
}

inline Quat Quat::operator*=(float s)
{
	x *= s; y *= s; z *= s; w *= s;
	return *this;
}

inline Quat Quat::operator*(const Quat& b) const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	return Quat 
		(
		w * b.w - x * b.x - y * b.y - z * b.z,
		w * b.x + x * b.w + y * b.z - z * b.y,
		w * b.y + y * b.w + z * b.x - x * b.z,
		w * b.z + z * b.w + x * b.y - y * b.x
		);
}

inline void Quat::toEuler(Vec3& euler) const
{
	const double sqw = w*w;
	const double sqx = x*x;
	const double sqy = y*y;
	const double sqz = z*z;
	const double test = 2.0 * (y*w - x*z);

	if (test < 1.0 + 0.000001 && x>1.0 - 0.000001)
	{
		// heading = rotation about z-axis
		euler.z = (float) (-2.0*atan2(x, w));
		// bank = rotation about x-axis
		euler.x = 0;
		// attitude = rotation about y-axis
		euler.y = (float) (QUAT_PI/2.0);
	}
	else if (test < -1.0 + 0.000001 && x> -1.0 - 0.000001)
	{
		// heading = rotation about z-axis
		euler.z = (float) (2.0*atan2(x, w));
		// bank = rotation about x-axis
		euler.x = 0;
		// attitude = rotation about y-axis
		euler.y = (float) (QUAT_PI/-2.0);
	}
	else
	{
		// heading = rotation about z-axis
		euler.z = (float) atan2(2.0 * (x*y +z*w),(sqx - sqy - sqz + sqw));
		// bank = rotation about x-axis
		euler.x = (float) atan2(2.0 * (y*z +x*w),(-sqx - sqy + sqz + sqw));
		// attitude = rotation about y-axis
		double t = test > 1.0 ? 1.0 : test;
		t = t < -1.0 ? -1.0 : t;
		euler.y = (float) asin( t );
	}
}

