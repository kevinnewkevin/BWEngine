#pragma once
#include "base/Ref.h"

class GameObject;

class BaseComponent
{
public:
	BaseComponent(GameObject* gameObject = nullptr);
	virtual ~BaseComponent();

public:
	GameObject* gameObject = nullptr;

protected:

};

