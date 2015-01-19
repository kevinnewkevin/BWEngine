#include "mat4.h"
#include "quat.h"

const Mat4 Mat4::IDENTITY = Mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

const Mat4 Mat4::ZERO = Mat4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0);


Mat4::Mat4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

void Mat4::set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	m[0] = m11;
	m[1] = m21;
	m[2] = m31;
	m[3] = m41;
	m[4] = m12;
	m[5] = m22;
	m[6] = m32;
	m[7] = m42;
	m[8] = m13;
	m[9] = m23;
	m[10] = m33;
	m[11] = m43;
	m[12] = m14;
	m[13] = m24;
	m[14] = m34;
	m[15] = m44;
}

void Mat4::makeIdentity()
{
	memset(m, 0, sizeof(float)* 16);   // memory.h
	m[0] = m[5] = m[10] = m[15] = 1.f;
}

void Mat4::buildPerspectiveProjection(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	float cotangent = (float)(cos(fieldOfViewRadians/2) / sin(fieldOfViewRadians/2));

	m[0] = cotangent / aspectRatio;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = cotangent;
	m[6] = 0;
	m[7] = 0;
	
	m[8] = 0;
	m[9] = 0;
	m[10] = (zFar+zNear)/(zNear-zFar);
//	M[10] = (zFar/(zNear-zFar)); // DirectX version
	m[11] = -1;

	m[12] = 0;
	m[13] = 0;
	m[14] = 2*zNear*zFar/(zNear-zFar);
//	M[14] = (zNear*zFar/(zNear-zFar)); // DirectX version
	m[15] = 0;
}

void Mat4::transformVec3(float& x, float& y, float& z)
{
	float vector[3];

	vector[0] = x*m[0] + y*m[4] + z*m[8] + m[12];
	vector[1] = x*m[1] + y*m[5] + z*m[9] + m[13];
	vector[2] = x*m[2] + y*m[6] + z*m[10] + m[14];

	x = vector[0];
	y = vector[1];
	z = vector[2];
}

void Mat4::setScale(float x, float y, float z)
{
	m[0] = x;
	m[5] = y;
	m[10] = z;
}

void Mat4::multiplyWith1x4Matrix(float *matrix)
{
	float mat[4];
	mat[0] = matrix[0];
	mat[1] = matrix[1];
	mat[2] = matrix[2];
	mat[3] = matrix[3];

	matrix[0] = m[0]*mat[0] + m[4]*mat[1] + m[8]*mat[2] + m[12]*mat[3];
	matrix[1] = m[1]*mat[0] + m[5]*mat[1] + m[9]*mat[2] + m[13]*mat[3];
	matrix[2] = m[2]*mat[0] + m[6]*mat[1] + m[10]*mat[2] + m[14]*mat[3];
	matrix[3] = m[3]*mat[0] + m[7]*mat[1] + m[11]*mat[2] + m[15]*mat[3];
}

Mat4 Mat4::inverse()
{
	Mat4 r;
	float v0 = m[2] * m[7] - m[6] * m[3];
    float v1 = m[2] * m[11] - m[10] * m[3];
    float v2 = m[2] * m[15] - m[14] * m[3];
    float v3 = m[6] * m[11] - m[10] * m[7];
    float v4 = m[6] * m[15] - m[14] * m[7];
    float v5 = m[10] * m[15] - m[14] * m[11];

    float t00 = + (v5 * m[5] - v4 * m[9] + v3 * m[13]);
    float t10 = - (v5 * m[1] - v2 * m[9] + v1 * m[13]);
    float t20 = + (v4 * m[1] - v2 * m[5] + v0 * m[13]);
    float t30 = - (v3 * m[1] - v1 * m[5] + v0 * m[9]);

    float invDet = 1 / (t00 * m[0] + t10 * m[4] + t20 * m[8] + t30 * m[12]);

    r.m[0] = t00 * invDet;
    r.m[1] = t10 * invDet;
    r.m[2] = t20 * invDet;
    r.m[3] = t30 * invDet;

    r.m[4] = - (v5 * m[4] - v4 * m[8] + v3 * m[12]) * invDet;
    r.m[5] = + (v5 * m[0] - v2 * m[8] + v1 * m[12]) * invDet;
    r.m[6] = - (v4 * m[0] - v2 * m[4] + v0 * m[12]) * invDet;
    r.m[7] = + (v3 * m[0] - v1 * m[4] + v0 * m[8]) * invDet;

    v0 = m[1] * m[7] - m[5] * m[3];
    v1 = m[1] * m[11] - m[9] * m[3];
    v2 = m[1] * m[15] - m[13] * m[3];
    v3 = m[5] * m[11] - m[9] * m[7];
    v4 = m[5] * m[15] - m[13] * m[7];
    v5 = m[9] * m[15] - m[13] * m[11];

    r.m[8] = + (v5 * m[4] - v4 * m[8] + v3 * m[12]) * invDet;
    r.m[9] = - (v5 * m[0] - v2 * m[8] + v1 * m[12]) * invDet;
    r.m[10] = + (v4 * m[0] - v2 * m[4] + v0 * m[12]) * invDet;
    r.m[11] = - (v3 * m[0] - v1 * m[4] + v0 * m[8]) * invDet;

    v0 = m[6] * m[1] - m[2] * m[5];
    v1 = m[10] * m[1] - m[2] * m[9];
    v2 = m[14] * m[1] - m[2] * m[13];
    v3 = m[10] * m[5] - m[6] * m[9];
    v4 = m[14] * m[5] - m[6] * m[13];
    v5 = m[14] * m[9] - m[10] * m[13];

    r.m[12] = - (v5 * m[4] - v4 * m[8] + v3 * m[12]) * invDet;
    r.m[13] = + (v5 * m[0] - v2 * m[8] + v1 * m[12]) * invDet;
    r.m[14] = - (v4 * m[0] - v2 * m[4] + v0 * m[12]) * invDet;
    r.m[15] = + (v3 * m[0] - v1 * m[4] + v0 * m[8]) * invDet;
	return r;
}

void Mat4::buildTranslate(const Vec3& v)
{
	buildTranslate(v.x, v.y, v.z);
}

void Mat4::buildTranslate(float x, float y, float z)
{
	makeIdentity();
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

void Mat4::buildScale(const Vec3 & v)
{
	buildScale(v.x, v.y, v.z);
}

void Mat4::buildScale(float x, float y, float z)
{
	makeIdentity();

	this->m[0] = x;
	this->m[5] = y;
	this->m[10] = z;
}

void Mat4::buildRotation(const Vec3 & v)
{
	buildRotation(v.x, v.y, v.z);
}

void Mat4::buildRotation(const Quat & q)
{
	float x2 = q.x + q.x;
	float y2 = q.y + q.y;
	float z2 = q.z + q.z;

	float xx2 = q.x * x2;
	float yy2 = q.y * y2;
	float zz2 = q.z * z2;
	float xy2 = q.x * y2;
	float xz2 = q.x * z2;
	float yz2 = q.y * z2;
	float wx2 = q.w * x2;
	float wy2 = q.w * y2;
	float wz2 = q.w * z2;

	m[0] = 1.0f - yy2 - zz2;
	m[1] = xy2 + wz2;
	m[2] = xz2 - wy2;
	m[3] = 0.0f;

	m[4] = xy2 - wz2;
	m[5] = 1.0f - xx2 - zz2;
	m[6] = yz2 + wx2;
	m[7] = 0.0f;

	m[8] = xz2 + wy2;
	m[9] = yz2 - wx2;
	m[10] = 1.0f - xx2 - yy2;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void Mat4::buildRotation(float x, float y, float z)
{
	Mat4 rotX, rotY, rotZ;
	rotX.buildRotationX(x);
	rotY.buildRotationY(y);
	rotZ.buildRotationZ(z);
	makeIdentity();
	*this *= rotZ;
	*this *= rotX;
	*this *= rotY;
}

void Mat4::buildRotationX(float angle)
{
	makeIdentity();
	float c = cos(angle);
	float s = sin(angle);

	this->m[5] = c;
	this->m[6] = s;
	this->m[9] = -s;
	this->m[10] = c;
}

void Mat4::buildRotationY(float angle)
{
	makeIdentity();
	float c = cos(angle);
	float s = sin(angle);

	this->m[0] = c;
	this->m[2] = -s;
	this->m[8] = s;
	this->m[10] = c;
}

void Mat4::buildRotationZ(float angle)
{
	makeIdentity();
	float c = cos(angle);
	float s = sin(angle);

	this->m[0] = c;
	this->m[1] = s;
	this->m[4] = -s;
	this->m[5] = c;
}

Mat4 Mat4::operator*(const Mat4& mat)
{
	Mat4 r;
	const float* m1 = mat.m;
	r.m[0] = m[0] * m1[0] + m[4] * m1[1] + m[8] * m1[2] + m[12] * m1[3];
	r.m[1] = m[1] * m1[0] + m[5] * m1[1] + m[9] * m1[2] + m[13] * m1[3];
	r.m[2] = m[2] * m1[0] + m[6] * m1[1] + m[10] * m1[2] + m[14] * m1[3];
	r.m[3] = m[3] * m1[0] + m[7] * m1[1] + m[11] * m1[2] + m[15] * m1[3];
	r.m[4] = m[0] * m1[4] + m[4] * m1[5] + m[8] * m1[6] + m[12] * m1[7];
	r.m[5] = m[1] * m1[4] + m[5] * m1[5] + m[9] * m1[6] + m[13] * m1[7];
	r.m[6] = m[2] * m1[4] + m[6] * m1[5] + m[10] * m1[6] + m[14] * m1[7];
	r.m[7] = m[3] * m1[4] + m[7] * m1[5] + m[11] * m1[6] + m[15] * m1[7];
	r.m[8] = m[0] * m1[8] + m[4] * m1[9] + m[8] * m1[10] + m[12] * m1[11];
	r.m[9] = m[1] * m1[8] + m[5] * m1[9] + m[9] * m1[10] + m[13] * m1[11];
	r.m[10] = m[2] * m1[8] + m[6] * m1[9] + m[10] * m1[10] + m[14] * m1[11];
	r.m[11] = m[3] * m1[8] + m[7] * m1[9] + m[11] * m1[10] + m[15] * m1[11];
	r.m[12] = m[0] * m1[12] + m[4] * m1[13] + m[8] * m1[14] + m[12] * m1[15];
	r.m[13] = m[1] * m1[12] + m[5] * m1[13] + m[9] * m1[14] + m[13] * m1[15];
	r.m[14] = m[2] * m1[12] + m[6] * m1[13] + m[10] * m1[14] + m[14] * m1[15];
	r.m[15] = m[3] * m1[12] + m[7] * m1[13] + m[11] * m1[14] + m[15] * m1[15];

	return r;
}

Mat4& Mat4::operator*=(const Mat4 & mat)
{
	Mat4&& r = std::move(*this * mat);
	memcpy(m, r.m, MATRIX_SIZE);
	return *this;
}

Mat4 Mat4::buildLookAt(Vec3 position, Vec3 target, Vec3 up)
{
	// right hand  opengl
	Vec3 zaxis = position - target;
	// left hand
	// vec3 zaxis = target - position;
	zaxis.normalize();

	Vec3 xaxis = up.cross(zaxis);
	xaxis.normalize();

	Vec3 yaxis = zaxis.cross(xaxis);

	m[0] = xaxis.x;
	m[1] = yaxis.x;
	m[2] = zaxis.x;
	m[3] = 0;

	m[4] = xaxis.y;
	m[5] = yaxis.y;
	m[6] = zaxis.y;
	m[7] = 0;

	m[8] = xaxis.z;
	m[9] = yaxis.z;
	m[10] = zaxis.z;
	m[11] = 0;

	m[12] = -xaxis.dot(position);
	m[13] = -yaxis.dot(position);
	m[14] = -zaxis.dot(position);
	m[15] = 1;

	return *this;
}