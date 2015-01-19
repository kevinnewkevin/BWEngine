#pragma once

#include <math.h>

#define GRAD_PI2 3.1415926	
	//! 3d vector template class with lots of operators and methods.
class Vec3  
{
public:

	Vec3(): x(0), y(0), z(0) {};
	Vec3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {};
	Vec3(const Vec3& other)	:x(other.x), y(other.y), z(other.z) {};

	// operators

	Vec3& operator=(const Vec3& other)	{ x = other.x; y = other.y; z = other.z; return *this; }

	Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z);	}
	Vec3& operator+=(const Vec3& other)	{ x+=other.x; y+=other.y; z+=other.z; return *this; }

	Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z);	}
	Vec3& operator-=(const Vec3& other)	{ x-=other.x; y-=other.y; z-=other.z; return *this; }

	Vec3 operator*(const Vec3& other) const { return Vec3(x * other.x, y * other.y, z * other.z);	}
	Vec3& operator*=(const Vec3& other)	{ x*=other.x; y*=other.y; z*=other.z; return *this; }
	Vec3 operator*(const float v) const { return Vec3(x * v, y * v, z * v);	}
	Vec3& operator*=(const float v) { x*=v; y*=v; z*=v; return *this; }

	Vec3 operator/(const Vec3& other) const { return Vec3(x / other.x, y / other.y, z / other.z);	}
	Vec3& operator/=(const Vec3& other)	{ x/=other.x; y/=other.y; z/=other.z; return *this; }
	Vec3 operator/(const float v) const { float i=(float)1.0/v; return Vec3(x * i, y * i, z * i);	}
	Vec3& operator/=(const float v) { float i=(float)1.0/v; x*=i; y*=i; z*=i; return *this; }

	bool operator<=(const Vec3&other) const { return x<=other.x && y<=other.y && z<=other.z;};
	bool operator>=(const Vec3&other) const { return x>=other.x && y>=other.y && z>=other.z;};

	bool operator==(const Vec3& other) const { return other.x==x && other.y==y && other.z==z; }
	bool operator!=(const Vec3& other) const { return other.x!=x || other.y!=y || other.z!=z; }

	// functions

	void set(const float nx, const float ny, const float nz) {x=nx; y=ny; z=nz; }
	void set(const Vec3& p) { x=p.x; y=p.y; z=p.z;}

	//! Returns length of the vector.
	float getLength() const { return sqrt(x*x + y*y + z*z); }

	//! Returns the dot product with another vector.
	float dot(const Vec3& other) const
	{
		return x*other.x + y*other.y + z*other.z;
	}

	//! Returns distance from an other point. Here, the vector is interpreted as
	//! point in 3 dimensional space.
	float getDistanceFrom(const Vec3& other)
	{
		float vx = x - other.x; float vy = y - other.y; float vz = z - other.z;
		return sqrt(vx*vx + vy*vy + vz*vz);
	}

	Vec3 cross(const Vec3& p) const
	{
		return Vec3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
	}

	bool isBetweenPoints(const Vec3& begin, const Vec3& end) const
	{
		// this is very slow, i'll have to write a faster one later.

		Vec3 lv = end - begin;
		Vec3 pv = *this - begin;

		float l1 = lv.x*lv.x + lv.y*lv.y + lv.z*lv.z;
		float l2 = pv.x*pv.x + pv.y*pv.y + pv.z*pv.z;

		return (l2 < l1);
	}

	//! Normalizes the vector.
	Vec3 normalize()
	{
		float inv = (float)1.0 / (float)getLength();
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	}

	//! Inverts the vector.
	void invert()
	{
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	void rotateXZBy(float degrees, const Vec3& center)
	{
		degrees *= (float)GRAD_PI2;
		float cs = (float)cos(degrees);
		float sn = (float)sin(degrees);
		x -= center.x;
		z -= center.z;
		set(x*cs - z*sn, y, x*sn + z*cs);
		x += center.x;
		z += center.z;
	}

	void rotateXYBy(float degrees, const Vec3& center)
	{
		degrees *= (float)GRAD_PI2;
		float cs = (float)cos(degrees);
		float sn = (float)sin(degrees);
		x -= center.x;
		y -= center.y;
		set(x*cs - y*sn, x*sn + y*cs, z);
		x += center.x;
		y += center.y;
	}

	void rotateYZBy(float degrees, const Vec3& center)
	{
		degrees *= (float)GRAD_PI2;
		float cs = (float)cos(degrees);
		float sn = (float)sin(degrees);
		z -= center.z;
		y -= center.y;
		set(x, y*cs - z*sn, y*sn + z*cs);
		z += center.z;
		y += center.y;
	}

	// member variables

	float x, y, z;
};


