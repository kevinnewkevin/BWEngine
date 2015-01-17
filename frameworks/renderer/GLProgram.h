#pragma once
#include "base/Ref.h"
#include "gl/glew.h"

class GLProgram : Ref
{
public:
	GLuint program = 0;
	GLuint worldLocation = 0;
	GLuint sampler = 0;

public:
	GLProgram();
	~GLProgram();

	void init();
	GLuint initWithFile(const char* shaderFile);
	GLuint initWithContent(unsigned char* shader, int size = 0);
	GLuint initWithFile(const char* frag, const char* vert);
	GLuint initWithContent(const char* frag, const char* vert);

protected:
	GLuint loadShaderWithType(GLenum shaderType, const char* source);

};

