#pragma once
#include "renderer/command/RenderCommand.h"
#include "renderer/material/MeshMaterial.h"

class MeshCommand : public RenderCommand
{
public:
	MeshCommand() { type = TYPE::MESH; }
	MeshCommand(MeshMaterial* $material, Mat4* $mat)
		: RenderCommand($material, $mat)
	{
		type = TYPE::MESH;
	}

	void init(MeshMaterial* $material, Mat4* $mat);

	virtual ~MeshCommand() {}

public:

};