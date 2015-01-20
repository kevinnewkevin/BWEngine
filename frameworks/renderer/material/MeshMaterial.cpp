#include "renderer/material/MeshMaterial.h"
#include "renderer/GLProgramCache.h"

MeshMaterial * MeshMaterial::create(GLProgram * glprogram)
{
	MeshMaterial* ret = nullptr;
	ret = new (std::nothrow) MeshMaterial();

	if (glprogram == nullptr)
	{
		glprogram = GLProgramCache::getInstance()->getGLProgram(GLProgramCache::NAME::POSITION_TEXTURE);
	}

	if (ret && ret->init(glprogram))
	{
		ret->autoRelease();
		return ret;
	}
	SAFE_DELETE(ret);
	return nullptr;
}

