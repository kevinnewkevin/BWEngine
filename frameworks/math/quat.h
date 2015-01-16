#pragma once

#include "vec3.h"
#include "mat4.h"
#define quat_PI 3.1415926

class quat
{
public:
	quat() :w(1), x(0), y(0), z(0) {};
	quat(float ww, float xx, float yy, float zz);
	~quat() {};

	// add operator
	inline quat operator+(const quat& b) const;
	inline quat operator* (float fScalar) const;
	inline quat operator*=(float s);


	float dot(const quat& q) const;
	mat4  toMatrix() const;
	void  fromAxisAngle(const vec3& axis, float angle);
	quat& slerp(quat q1, quat q2, float time, float threshold = 1.0);
	quat& lerp(quat q1, quat q2, float time);

	float x, y, z, w;
};

inline quat quat::operator+(const quat& b) const
{
	return quat(w + b.w, x + b.x, y + b.y, z + b.z);
}

inline quat quat::operator* (float fscalar) const
{
	return quat(fscalar*w, fscalar*x, fscalar*y, fscalar*z);
}

inline quat quat::operator*=(float s)
{
	x *= s; y *= s; z *= s; w *= s;
	return *this;
}

typedef quat Quat;