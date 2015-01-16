#pragma once
#include <windows.h>
#include "GL/glew.h"
class GLView
{
public:
	GLView();
	~GLView();

	bool init(char* title, int width, int height, int bits, bool fullscreenflag);
	GLvoid killGLWindow();
	
	bool processEvents();

	void end();

	HDC			mDC;		// Private GDI Device Context
	HGLRC		mRC;		// Permanent Rendering Context
	HWND		mWnd;		// Holds Our Window Handle
	HINSTANCE	mInstance;		// Holds The Instance Of The Application
	bool        mShutDown;
};

