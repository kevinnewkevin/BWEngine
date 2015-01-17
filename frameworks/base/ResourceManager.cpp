#include "base/ResourceManager.h"
#include "renderer/Image.h"
#include "renderer/Texture.h"
#include "renderer/ImageLoader.h"
#include "utils/FileUtils.h"

ResourceManager* ResourceManager::sm_pSharedInstance = nullptr;
ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	
}

ResourceManager* ResourceManager::getInstance()
{
	if (sm_pSharedInstance == NULL)
	{
		sm_pSharedInstance = new ResourceManager();
	}
	return sm_pSharedInstance;
}

Texture* ResourceManager::addTexture(const char* file)
{
	Texture* texture = NULL;
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(file);
	auto it = _textures.find(fullPath);
    if( it != _textures.end() ) {
		texture = it->second;
		return texture;
    }

	Image image;
	loadImagePNG(fullPath.c_str(), image);
	texture = new Texture();
	texture->initWithImage(&image);

	_textures.insert(std::make_pair(fullPath, texture));

	CheckGLError();
	return texture;
}