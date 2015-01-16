#include "quat.h"

quat::quat(float ww, float xx, float yy, float zz) :w(ww), x(xx), y(yy), z(zz)
{

}

void quat::fromAxisAngle(const vec3& axis, float angle)
{
	float rad = angle*0.5f;
	float scale = sinf(rad);

	w = cos(rad);
	x = axis.x * scale;
	y = axis.y * scale;
	z = axis.z * scale;
}

mat4 quat::toMatrix() const
{
	mat4 m;
	float wx, wy, wz, xx, xy, xz, yy, yz, zz;

	// calculate coefficients
	xx = x * x * 2.f;
	xy = x * y * 2.f;
	xz = x * z * 2.f;
	
	yy = y * y * 2.f;
	yz = y * z * 2.f;
	
	zz = z * z * 2.f;

	wx = w * x * 2.f;
	wy = w * y * 2.f;
	wz = w * z * 2.f;

	m.m[0]	= 1.0f - (yy + zz);
	m.m[4]	= xy - wz;
	m.m[8]	= xz + wy;
	m.m[12] = 0;

	m.m[1]  = xy + wz;
	m.m[5]  = 1.0f - (xx + zz);
	m.m[9]  = yz - wx;
	m.m[13] = 0;

	m.m[2] = xz - wy;
	m.m[6] = yz + wx;
	m.m[10] = 1.0f - (xx + yy);
	m.m[14]= 0;

	m.m[3] = 0;
	m.m[7] = 0;
	m.m[11] = 0;
	m.m[15] = 1;
	return m;
}

float quat::dot(const quat& rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}

quat& quat::slerp(quat q1, quat q2, float time, float threshold)
{
	float angle = q1.dot(q2);

	// make sure we use the short rotation
	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1-threshold)) // spherical interpolation
	{
		const float theta = acosf(angle);
		const float invsintheta = 1/(sinf(theta));
		const float scale = sinf(theta * (1.0f-time)) * invsintheta;
		const float invscale = sinf(theta * time) * invsintheta;
		return (*this = (q1*scale) + (q2*invscale));
	}
	else // linear interploation
		return lerp(q1,q2,time);
}

quat& quat::lerp(quat q1, quat q2, float time)
{
	const float scale = 1.0f - time;
	return (*this = (q1*scale) + (q2*time));
}