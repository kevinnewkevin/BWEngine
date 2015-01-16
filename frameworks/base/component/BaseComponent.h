#pragma once
#include "base/Ref.h"

class GameObject;

class BaseComponent : public Ref
{
public:
	BaseComponent(GameObject* gameObject = nullptr);
	virtual ~BaseComponent();

	virtual void Awake();
	virtual void Start();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	virtual void Update(float dt);

	virtual void OnGUI();

	void setEnabled(bool enable);
	bool isEnabled();

	bool canDraw();

public:
	GameObject* gameObject = nullptr;

protected:
	bool _enable = true;
	bool _isStarted = false;
	bool _canDraw = false;
};

