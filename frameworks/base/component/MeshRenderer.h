#pragma once
#include "base/component/BaseComponent.h"
#include "renderer/material/MeshMaterial.h"
#include "renderer/command/MeshCommand.h"
#include <string>

using std::string;

class GameObject;
class Texture;
class Mesh;
class SkinnedMesh;

class MeshRenderer : public BaseComponent
{
public:
	MeshRenderer(GameObject* gameObject = nullptr);
	virtual ~MeshRenderer();

	void setFile(const char* file);
	virtual void Awake();
	virtual void Start();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	virtual void Update(float dt);

	virtual void OnGUI();

	SkinnedMesh* getSkin();

public:

	MeshMaterial* material = nullptr;
	string _filePath;
	Mesh*  _mesh;
	SkinnedMesh* _skinnedMesh;
	Texture* _texture;
protected:
	MeshCommand _cmd;
};

