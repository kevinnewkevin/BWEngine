#include "base/component/Transform.h"
#include "base/GameObject.h"


Transform::Transform(GameObject* owner/* = nullptr*/)
: BaseComponent(owner), _scale(vec3(1.0, 1.0, 1.0))
{
}


Transform::~Transform()
{
	_children.clear();
}

void Transform::Update(float dt)
{
	BaseComponent::Update(dt);
	apply();
}

Vector<Transform*>& Transform::getChildren()
{
	return _children;
}

void Transform::setPosition(const Vec3 & pos)
{
	_position = pos;
	_transformDirty = true;
}

void Transform::setScale(const Vec3 & scale)
{
	_scale = scale;
	_transformDirty = true;
}

void Transform::setRotation(const Vec3 & rot)
{
	_rotation = rot;
	_transformDirty = true;
}

void Transform::setLocalPosition(const Vec3 & pos)
{
	_localPosition = pos;
	_transformLocalDirty = true;
}

void Transform::setLocalScale(const Vec3 & scale)
{
	_localScale = scale;
	_transformLocalDirty = true;
}

void Transform::setLocalRotation(const Vec3 & rot)
{
	_localRotation = rot;
	_transformLocalDirty = true;
}

void Transform::setParent(Transform* parent) {
	if (_parent != parent)
	{
		_parent = parent;
		if (_parent)
		{
			gameObject->_parent = _parent->gameObject;
		}
		else
		{
			gameObject->_parent = nullptr;
		}
		_transformDirty = true;
	}
}

const Mat4& Transform::apply()
{
	if (_transformLocalDirty)
	{

		_transformLocalDirty = false;
	}
	if (_transformDirty)
	{
		Mat4 scaleMat, rotMat, transMat;
		scaleMat.buildScale(_scale);
		transMat.buildTranslate(_position);
		rotMat.buildRotation(_rotation);

		_modleMatrix.set(scaleMat);
		_modleMatrix *= rotMat;
		_modleMatrix *= transMat;

		_transformDirty = false;
	}
	return _modleMatrix;
}
