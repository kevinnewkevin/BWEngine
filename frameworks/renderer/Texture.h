#pragma once
#include "gl/glew.h"
#include "math/vec2.h"

class  Image;

class Texture
{
public:
	Texture() {}
	~Texture() {}
	
	bool initWithImage(Image* i);

	void begin();

	void end();
private:

    int _pixelsWide;

    int _pixelsHigh;

    GLuint _name;

    /** texture max S */
    GLfloat _maxS;
    
    GLfloat _maxT;

    Vec2 _contentSize;

    bool _hasPremultipliedAlpha;

    bool _hasMipmaps;

    bool _antialiasEnabled;

};