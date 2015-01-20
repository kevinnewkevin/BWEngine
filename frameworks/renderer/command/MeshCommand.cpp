#include "MeshCommand.h"

void MeshCommand::init(MeshMaterial * $material, Mat4 * $mat)
{
	material = $material;
	mat = $mat;
	type = TYPE::MESH;
}
