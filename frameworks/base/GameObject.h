#pragma once
#include "base/Ref.h"
#include "base/component/Transform.h"

class GameObject : public Ref//, public BaseMSGClass
{
	friend class Transform;
public:
	GameObject();
	virtual ~GameObject();

	// component
public:
	Transform* getTransform();

	// getter && setter
public:
	void setParent(GameObject* parent);
	GameObject* getParent();

protected:
	Transform* _transform = nullptr;
	GameObject* _parent = nullptr;
};

