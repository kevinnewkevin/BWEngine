#include "Camera.h"
#include "math/vec3.h"
#include "math/quat.h"
#include "math/mat4.h"
#include <vector>
#include "base/GameObject.h"
#include "renderer/GLProgramCache.h"
#include "renderer/Renderer.h"
using std::vector;

Camera* Camera::main = nullptr;

Camera::Camera()
{
	if (main == nullptr) main = this;
}

Camera::~Camera()
{

}

void Camera::Start()
{
	gameObject->getTransform()->setPosition(Vec3(0, 0, 0));
	_LookAtMatrix.buildLookAt(Vec3(0, 0, 700), Vec3(0, 0, 0), Vec3(0, 1, 0));

	_PerspectiveMatrix.buildPerspectiveProjection((float)QUAT_PI / 3, 800.0f / 600, 1, 1000);

	traverse(gameObject->getTransform());

	glViewport(0, 0, 800, 600);
}

void Camera::traverse(Transform* t)
{
	for (unsigned int i = 0; i < t->getChildren().size(); ++i)
	{
		auto child = t->getChildren().at(i);
		//if (child->canDraw())
		{
			GameObject* obj = t->getChildren().at(i)->gameObject;
			_RenderObject.push_back(obj);
			traverse(obj->getTransform());
		}
	}
}

void Camera::OnGUI()
{
	CheckGLError();
	Mat4 cameraTrans = gameObject->getTransform()->apply();
	cameraTrans.m[12] *= -1;
	cameraTrans.m[13] *= -1;
	cameraTrans.m[14] *= -1;
	projectMat = _PerspectiveMatrix * _LookAtMatrix * cameraTrans;

	for (unsigned int i = 0; i < _RenderObject.size(); ++i)
	{
		GameObject* obj = _RenderObject[i];
		obj->mvpMatrix = projectMat * obj->getTransform()->apply();

		auto& components = obj->getComponents();
		for (auto comp : components)
		{
			comp->OnGUI();
		}
	}

	Renderer::getInstance()->render();
}