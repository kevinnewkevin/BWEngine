#pragma once
#include "renderer/material/Material.h"
#include "renderer/Texture.h"
#include "renderer/Mesh.h"

class MeshMaterial : public Material
{
public:
	MeshMaterial() {}
	virtual ~MeshMaterial() {}
	
	static MeshMaterial* create(GLProgram* glprogram = nullptr);

public:
	Texture* texture = nullptr;
	vector<Vec3>* positions = nullptr;
	vector<Vec3>* normals = nullptr;
	vector<Vec2>* texCoords = nullptr;
	vector<unsigned short>* indices = nullptr;
};