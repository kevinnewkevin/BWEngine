#pragma once
#include <string>
#include <unordered_map>
#include "renderer/GLProgram.h"

class GLProgram;

class GLProgramCache : public Ref
{
public:
	struct TYPE {
		enum {
			POSITION_TEXTURE
		};
	};
	struct NAME {
		static const char* POSITION_TEXTURE;
	};

public:

    GLProgramCache();

	virtual ~GLProgramCache();

    static GLProgramCache* getInstance();

    static void purge();

    void loadDefaultGLPrograms();

    void reloadDefaultGLPrograms();

    GLProgram * getGLProgram(const std::string &key);

    void addGLProgram(GLProgram* program, const std::string &key);

private:
    bool init();
    void loadDefaultGLProgram(GLProgram *program, int type);
    
    std::string getShaderMacrosForLight();

    std::unordered_map<std::string, GLProgram*> _programs;
};