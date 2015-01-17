#pragma once
#include "base/Ref.h"
#include "gl/glew.h"

class GLProgram : Ref
{
public:
	GLProgram();
	~GLProgram();

	void init();
	bool initWithFile(const char* frag, const char* vert);
	bool initWithContent(const char* frag, const char* vert);


	GLuint program;
	GLuint worldLocation;
	GLuint sampler;
};

