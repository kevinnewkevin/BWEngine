#pragma once
#include "base/component/BaseComponent.h"
#include "renderer/Material.h"

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

	void updateTexture();

public:
	Material* material = nullptr;
	Texture* texture = nullptr;

protected:
	std::vector<Vec3> vertices;
	std::vector<Vec2> texCoords;
	std::vector<unsigned short> indices;

	bool _isDirty = false;
};

