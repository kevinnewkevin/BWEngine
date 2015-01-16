#pragma once
#include "base/BaseClass.h"
#include <map>
using std::map;

class Texture;
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

    static ResourceManager* getInstance();

	Texture* addTexture(const char* file);

private:

    static ResourceManager* sm_pSharedInstance;


	std::unordered_map<std::string, Texture*> _textures;
};
