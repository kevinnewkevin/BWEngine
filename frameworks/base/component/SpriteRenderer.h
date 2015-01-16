#pragma once
#include "base/component/BaseComponent.h"

class GameObject;
class Texture;

class SpriteRenderer : public BaseComponent
{
public:
	SpriteRenderer(GameObject* gameObject = nullptr);
	virtual ~SpriteRenderer();

	virtual void Awake();
	virtual void Start();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	virtual void Update(float dt);

	virtual void OnGUI();
public:

	Texture* _texture;
protected:
};

