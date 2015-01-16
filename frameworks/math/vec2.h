#pragma once
class vec2
{
public:
	~vec2();

	vec2(): x(0), y(0) {};
	vec2(float xx, float yy) : x(xx), y(yy){};
	vec2(const vec2& other)	:x(other.x), y(other.y) {};

	// operators

	vec2& operator=(const vec2& other)	{ x = other.x; y = other.y; return *this; }

	vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y);	}
	vec2& operator+=(const vec2& other)	{ x+=other.x; y+=other.y; return *this; }

	vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y);	}
	vec2& operator-=(const vec2& other)	{ x-=other.x; y-=other.y; return *this; }

	vec2 operator*(const vec2& other) const { return vec2(x * other.x, y * other.y);	}
	vec2& operator*=(const vec2& other)	{ x*=other.x; y*=other.y; return *this; }
	vec2 operator*(const float v) const { return vec2(x * v, y * v);	}
	vec2& operator*=(const float v) { x*=v; y*=v; return *this; }

	vec2 operator/(const vec2& other) const { return vec2(x / other.x, y / other.y);	}
	vec2& operator/=(const vec2& other)	{ x/=other.x; y/=other.y; return *this; }
	vec2 operator/(const float v) const { float i=(float)1.0/v; return vec2(x * i, y * i);	}
	vec2& operator/=(const float v) { float i=(float)1.0/v; x*=i; y*=i; return *this; }

	bool operator<=(const vec2&other) const { return x<=other.x && y<=other.y;};
	bool operator>=(const vec2&other) const { return x>=other.x && y>=other.y;};

	bool operator==(const vec2& other) const { return other.x==x && other.y==y; }
	bool operator!=(const vec2& other) const { return other.x!=x || other.y!=y; }

	// functions

	float x, y;

};

typedef vec2 Vec2;