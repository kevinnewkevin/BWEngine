#pragma once
#include "base/Ref.h"
#include "base/component/Transform.h"
#include "base/Vector.h"

class GameObject : public Ref//, public BaseMSGClass
{
	friend class Transform;
public:
	std::function<void(float)> updateCallback = nullptr;

public:
	GameObject(const char* name = nullptr);
	virtual ~GameObject();

public:
	void Update(float dt);

	// component
public:
	Transform* getTransform();
	void addComponent(BaseComponent*);
	void removeComponent(BaseComponent*);
	Vector<BaseComponent*>& getComponents() { return _comps; }
	// getter && setter
public:
	void setParent(GameObject* parent);
	GameObject* getParent();
	void addChild(GameObject* child);

public:
	Mat4 mvpMatrix;

protected:
	Transform* _transform = nullptr;
	GameObject* _parent = nullptr;
	std::string _name = "GameObject";

	Vector<BaseComponent*> _comps;
};

