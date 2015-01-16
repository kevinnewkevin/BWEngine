#include "base/GameObject.h"

GameObject::GameObject()
{
	_transform = new Transform(this);
}


GameObject::~GameObject()
{
	SAFE_DELETE(_transform);
}

Transform* GameObject::getTransform() 
{
	return _transform;
}

void GameObject::setParent(GameObject* parent) 
{
	_parent = parent;
	_transform->setParent(parent == nullptr ? nullptr : parent->getTransform());
}

GameObject * GameObject::getParent()
{
	return _parent;
}
