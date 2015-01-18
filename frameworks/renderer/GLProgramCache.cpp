#include "renderer/GLProgramCache.h"

const char* GLProgramCache::NAME::POSITION_TEXTURE = "POSITION_TEXTURE";

static GLProgramCache *_sharedGLProgramCache = 0;

GLProgramCache* GLProgramCache::getInstance()
{
    if (!_sharedGLProgramCache) {
        _sharedGLProgramCache = new (std::nothrow) GLProgramCache();
        if (!_sharedGLProgramCache->init())
        {
            SAFE_DELETE(_sharedGLProgramCache);
        }
    }
    return _sharedGLProgramCache;
}

void GLProgramCache::purge()
{
    SAFE_RELEASE_NULL(_sharedGLProgramCache);
}

GLProgramCache::GLProgramCache()
	: _programs()
{

}

GLProgramCache::~GLProgramCache()
{
    for( auto it = _programs.begin(); it != _programs.end(); ++it ) {
        (it->second)->release();
    }
}

bool GLProgramCache::init()
{    
    loadDefaultGLPrograms();
    return true;
}

void GLProgramCache::loadDefaultGLPrograms()
{
    GLProgram *p = new (std::nothrow) GLProgram();
	p->autoRelease();
    loadDefaultGLProgram(p, TYPE::POSITION_TEXTURE);
    _programs.insert( std::make_pair( p->getName(), p ) );
}

void GLProgramCache::reloadDefaultGLPrograms()
{
}

void GLProgramCache::loadDefaultGLProgram(GLProgram *p, int type)
{
	switch (type) {
        case TYPE::POSITION_TEXTURE:
            p->initWithFile("shaders/default.shader");
            break;
        default:
            LOG("cocos2d: %s:%d, error shader type", __FUNCTION__, __LINE__);
            return;
    }
    
    p->link();
    p->updateUniforms();
    CheckGLError();
}

GLProgram* GLProgramCache::getGLProgram(const std::string &key)
{
    auto it = _programs.find(key);
    if( it != _programs.end() )
        return it->second;
    return nullptr;
}

void GLProgramCache::addGLProgram(GLProgram* program, const std::string &key)
{
    if (program)
        program->retain();    
    _programs[key] = program;
}

std::string GLProgramCache::getShaderMacrosForLight()
{
	return "";
    /*GLchar def[256];
    sprintf(def, "\n#define MAX_DIRECTIONAL_LIGHT_NUM %d \n"
            "\n#define MAX_POINT_LIGHT_NUM %d \n"
            "\n#define MAX_SPOT_LIGHT_NUM %d \n"
            , Configuration::getInstance()->getMaxSupportDirLightInShader(), Configuration::getInstance()->getMaxSupportPointLightInShader(), Configuration::getInstance()->getMaxSupportSpotLightInShader());
    return std::string(def);*/
}
