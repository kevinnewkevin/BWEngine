#pragma once
#include "gl/glew.h"
class PipelinePT
{
public:
	PipelinePT();
	~PipelinePT();

	void init();
	void drawScene();

	GLuint mProgram;
	GLuint mWorldLocation;
	GLuint mSampler;
};

