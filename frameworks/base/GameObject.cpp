#include "base/GameObject.h"

GameObject::GameObject(const char* name/* = nullptr*/)
{
	_transform = new Transform(this);
	_transform->autoRelease();
	_comps.pushBack(_transform);

	if (name)
	{
		_name.assign(name);
	}
}


GameObject::~GameObject()
{
	_comps.clear();

	updateCallback = nullptr;
}

void GameObject::Update(float dt)
{
	if (updateCallback != nullptr) updateCallback(dt);

	auto compsLen = _comps.size();
	if (compsLen > 1)
	{
		// update components
		for (ssize_t idx = 1; idx < compsLen; ++idx)
		{
			auto comp = _comps.at(idx);
			if (comp->isEnabled())
			{
				comp->Update(dt);
			}
		}

		// update children
		auto& children = _transform->getChildren();
		auto chLen = children.size();
		for (ssize_t idx = 0; idx < chLen; ++idx)
		{
			auto child = children.at(idx)->gameObject;
			child->Update(dt);
		}
	}
}

Transform* GameObject::getTransform() 
{
	return _transform;
}

void GameObject::addComponent(BaseComponent * comp)
{
	if (comp->gameObject != nullptr)
	{
		LOG("component %p has already been added!", comp);
		return;
	}
	comp->gameObject = this;
	comp->Awake();
	_comps.pushBack(comp);
}

void GameObject::removeComponent(BaseComponent * comp)
{
	auto index = _comps.getIndex(comp);
	if (index != -1)
	{
		comp->gameObject = nullptr;
		_comps.erase(index);
	}
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

void GameObject::addChild(GameObject* child)
{
	getTransform()->getChildren().pushBack(child->getTransform());
}