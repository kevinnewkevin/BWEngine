#pragma once
#include "gl/glew.h"
#include "base/Value.h"

class  Image;

class Texture
{
public:
	Texture() {}
	~Texture() {}
	
	bool initWithImage(Image* i);

	void begin();

	void end();

	GLuint getName();
	const Size& getSize();

private:

    int _pixelsWide;

    int _pixelsHigh;

    GLuint _name;

    /** texture max S */
    GLfloat _maxS;
    
    GLfloat _maxT;

    Size _contentSize;

    bool _hasPremultipliedAlpha;

    bool _hasMipmaps;

    bool _antialiasEnabled;

};