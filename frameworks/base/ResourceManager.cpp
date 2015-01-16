#include "base/ResourceManager.h"
#include "renderer/Image.h"
#include "renderer/Texture.h"
#include "renderer/ImageLoader.h"

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
	for (GLint error = glGetError(); error; error
    = glGetError()) {
		LOG("err");
    }

	Texture* texture = NULL;
	auto it = _textures.find(file);
    if( it != _textures.end() ) {
		texture = it->second;
		return texture;
    }

	Image image;
	loadImagePNG(file, image);
	image.flipSurface();
	texture = new Texture();
	texture->initWithImage(&image);

	_textures.insert(std::make_pair(file, texture));


		for (GLint error = glGetError(); error; error
    = glGetError()) {
		LOG("err");
    }
	return texture;
}