#pragma once
#include "base/component/BaseComponent.h"
#include "math/mat4.h"

#include <vector>
using std::vector;

class Transform;
class GameObject;

class Camera : public BaseComponent
{
public:
	static Camera* main;

	Mat4 projectMat;
public:
	Camera();
	~Camera();

	void Start();

	void OnGUI();

	void traverse(Transform* t);

private:
	Mat4 _LookAtMatrix;
	Mat4 _PerspectiveMatrix;
	vector<GameObject*> _RenderObject;
};

