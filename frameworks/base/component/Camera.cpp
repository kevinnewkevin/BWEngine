#include "Camera.h"
#include "math/vec3.h"
#include "base/GameObject.h"
#include "math/quat.h"
#include <vector>
#include "renderer/GLProgram.h"
#include "gl/glew.h"
using std::vector;

GLProgram program;
Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::Start()
{
	program.init();

	gameObject->getTransform()->setPosition(Vec3(0, 200, 0));
	_LookAtMatrix.buildLookAt(Vec3(0, 0, 700), Vec3(0, 0, 0), Vec3(0, 1, 0));

	_PerspectiveMatrix.buildPerspectiveProjection((float)QUAT_PI / 3, 800 / 600, 1, 1000);


	traverse(gameObject->getTransform());

	glViewport(0, 0, 800, 600);
}

void Camera::traverse(Transform* t)
{
	for (unsigned int i = 0; i < t->getChildren().size(); ++i)
	{
		//if ( need render)
		{
			GameObject* p = t->getChildren().at(i)->gameObject;
			_RenderObject.push_back(p);
			traverse(p->getTransform());
		}
	}
}

void Camera::OnGUI()
{
	CheckGLError();
	Mat4 mat;
	Mat4 cameraTrans = gameObject->getTransform()->apply();
	cameraTrans.m[12] *= -1;
	cameraTrans.m[13] *= -1;
	cameraTrans.m[14] *= -1;
	mat = _PerspectiveMatrix * _LookAtMatrix * cameraTrans;
	glUseProgram(program.program);

	for (unsigned int i = 0; i < _RenderObject.size(); ++i)
	{
		GameObject* obj = _RenderObject[i];
		Mat4 m = obj->getTransform()->apply();
		Mat4 mvp = mat*m;
		glUniformMatrix4fv(program.worldLocation, 1, GL_FALSE, &mvp.m[0]);

		CheckGLError();

		auto& components = obj->getComponents();
		for (auto comp : components)
		{
			comp->OnGUI();
		}
	}
}