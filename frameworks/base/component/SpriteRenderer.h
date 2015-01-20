#pragma once
#include "base/component/BaseComponent.h"
#include "renderer/material/MeshMaterial.h"
#include "renderer/command/MeshCommand.h"
#include "renderer/Mesh.h"

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
	void setTextureRect(Rect& rect);

public:
	MeshMaterial* material = nullptr;
	Texture* texture = nullptr;

protected:
	Mesh _mesh;
	bool _isDirty = false;
	Rect _textureRect;
	MeshCommand _cmd;
};

