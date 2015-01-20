#pragma once

#include "math/vec3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct PositionKey
{
	Vec3 position;
	float frame;
};

struct ScaleKey
{
	Vec3 position;
	float frame;
};

struct RotationKey
{
	Quat rotation;
	float frame;
};

class Joint
{
public:
	string name;
	Vec3 position;
	Vec3 scale;
	Quat rotation;
	vector<Joint*> children;
	Joint* parent;

	vector<PositionKey> positionKeys;
	vector<RotationKey> rotationKeys;
	vector<ScaleKey>    scaleKeys;
	vector<int>         vertexIndices;
	vector<float>       vertexWeights;

	Mat4 globalAnimatedMatrix;
	Mat4 globalInvMatrix;

	Mat4 globalPosition;

	void print();

	void getPositionKeyFramesAtFrame(float frame, PositionKey* preKey, PositionKey* nextKey);
	void getScaleKeyFramesAtFrame(float frame, ScaleKey* preKey, ScaleKey* nextKey);
	void getRotationKeyFramesAtFrame(float frame, RotationKey* preKey, RotationKey* nextKey);
	void getFrame(float frame, Vec3& position, Quat& rotation);
};
