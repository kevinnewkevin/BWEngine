#pragma once

#include <vector>
#include <assert.h>
using std::vector;

#include <GL/glew.h>
#include "math/vec3.h"
#include "math/vec2.h"


class Mesh {
public:
	Mesh();
	~Mesh();



	vector<Vec3> _positions;
	vector<Vec3> _normals;
	vector<Vec2> _texCoords;
	vector<unsigned short> _indices;
};