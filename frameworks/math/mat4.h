#pragma once
#include <memory.h>
#include <math.h>
#include <functional>
#include "vec3.h"

#define MATRIX_SIZE ( sizeof(float) * 16)

/*
		|0  4  8 12|
  m =   |1  5  9 13|
        |2  6 10 14|
		|3  7 11 15|
*/

class Quat;

class Mat4
{
public:
	Mat4() { makeIdentity(); }
	Mat4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
	~Mat4() {}

	Mat4(Mat4&& mat) { *this = std::move(mat); }
	Mat4& operator = (Mat4&& mat) { this->set(mat); return *this; }
	Mat4& operator = (const Mat4& mat) { this->set(mat); return *this; }

	inline Mat4 operator*(const Mat4& mat);
	inline Mat4& operator*=(const Mat4& mat);

	/**
	* Sets the values of this matrix.
	*
	* @param m11 The first element of the first row.
	* @param m12 The second element of the first row.
	* @param m13 The third element of the first row.
	* @param m14 The fourth element of the first row.
	* @param m21 The first element of the second row.
	* @param m22 The second element of the second row.
	* @param m23 The third element of the second row.
	* @param m24 The fourth element of the second row.
	* @param m31 The first element of the third row.
	* @param m32 The second element of the third row.
	* @param m33 The third element of the third row.
	* @param m34 The fourth element of the third row.
	* @param m41 The first element of the fourth row.
	* @param m42 The second element of the fourth row.
	* @param m43 The third element of the fourth row.
	* @param m44 The fourth element of the fourth row.
	*/
	void set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);

	void Mat4::set(const float* mat)
	{
		memcpy(this->m, mat, MATRIX_SIZE);
	}

	void Mat4::set(const Mat4& mat)
	{
		memcpy(this->m, mat.m, MATRIX_SIZE);
	}

	void makeIdentity();
	void buildPerspectiveProjection(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);
	void transformVec3(float& x, float& y, float& z);
	void setScale(float x, float y, float z);
	void multiplyWith1x4Matrix(float *mat);

	Mat4 inverse();
	Mat4 buildLookAt(Vec3 position, Vec3 target, Vec3 up);

	void buildTranslate(const Vec3& v);
	void buildTranslate(float x, float y, float z);
	void buildScale(const Vec3& v);
	void buildScale(float x, float y, float z);
	void buildRotation(const Vec3& v);
	void buildRotation(const Quat& q);
	void buildRotation(float x, float y, float z);
	void buildRotationX(float angle);
	void buildRotationY(float angle);
	void buildRotationZ(float angle);
public:
	float m[16];

	/** equals to a matrix full of zeros */
	static const Mat4 ZERO;
	/** equals to the identity matrix */
	static const Mat4 IDENTITY;

};