#pragma once
#include "base/Ref.h"

class Material;
class Mat4;

class RenderCommand : public Ref
{
public:
	struct TYPE {
		enum {
			NONE = 0,
			SPRITE,
			MESH,
			CUSTOM,
		};
	};
public:
	RenderCommand(){}

	RenderCommand(Material* $material, Mat4* $mat) 
		: material($material)
		, mat($mat)
	{}

	virtual ~RenderCommand() {}

public:
	Material* material = nullptr;
	Mat4* mat = nullptr;
	int type = 0;
};