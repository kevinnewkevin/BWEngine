#pragma once
class Vec2
{
public:
	~Vec2();

	Vec2(): x(0), y(0) {};
	Vec2(float xx, float yy) : x(xx), y(yy){};
	Vec2(const Vec2& other)	:x(other.x), y(other.y) {};

	// operators

	Vec2& operator=(const Vec2& other)	{ x = other.x; y = other.y; return *this; }

	Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y);	}
	Vec2& operator+=(const Vec2& other)	{ x+=other.x; y+=other.y; return *this; }

	Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y);	}
	Vec2& operator-=(const Vec2& other)	{ x-=other.x; y-=other.y; return *this; }

	Vec2 operator*(const Vec2& other) const { return Vec2(x * other.x, y * other.y);	}
	Vec2& operator*=(const Vec2& other)	{ x*=other.x; y*=other.y; return *this; }
	Vec2 operator*(const float v) const { return Vec2(x * v, y * v);	}
	Vec2& operator*=(const float v) { x*=v; y*=v; return *this; }

	Vec2 operator/(const Vec2& other) const { return Vec2(x / other.x, y / other.y);	}
	Vec2& operator/=(const Vec2& other)	{ x/=other.x; y/=other.y; return *this; }
	Vec2 operator/(const float v) const { float i=(float)1.0/v; return Vec2(x * i, y * i);	}
	Vec2& operator/=(const float v) { float i=(float)1.0/v; x*=i; y*=i; return *this; }

	bool operator<=(const Vec2&other) const { return x<=other.x && y<=other.y;};
	bool operator>=(const Vec2&other) const { return x>=other.x && y>=other.y;};

	bool operator==(const Vec2& other) const { return other.x==x && other.y==y; }
	bool operator!=(const Vec2& other) const { return other.x!=x || other.y!=y; }

	// functions

	float x, y;

};