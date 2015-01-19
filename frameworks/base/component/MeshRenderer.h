#pragma once
#include "base/component/BaseComponent.h"
#include "renderer/Material.h"
#include <string>

using std::string;

class GameObject;
class Texture;
class Mesh;

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
public:

	Material* material = nullptr;
	string _filePath;
	Mesh*  _mesh;
	Texture* _texture;
protected:
};

