#include "GLView.h"
bool	keys[256];			
bool	active=TRUE;		
bool	fullscreen=TRUE;	

#define _WIN_CLASS_NAME L"OpenGL" 

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
//			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


int InitGL(GLvoid)										
{
	glewInit();
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	glActiveTexture(GL_TEXTURE0);

	return TRUE;										
}

int DrawGLScene(GLvoid)									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									
	return TRUE;										
}

GLView::GLView()
: mShutDown(false), mDC(NULL), mRC(NULL), mWnd(NULL), mInstance(NULL)
{
	
}

GLView::~GLView()
{

}

GLvoid GLView::killGLWindow()								
{
	if (fullscreen)										
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);								
	}

	if (mRC)											
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
		}

		if (!wglDeleteContext(mRC))						// Are We Able To Delete The RC?
		{
		}
		mRC=NULL;										// Set RC To NULL
	}

	if (mDC && !ReleaseDC(mWnd, mDC))					// Are We Able To Release The DC
	{
		mDC=NULL;										// Set DC To NULL
	}

	if (mWnd && !DestroyWindow(mWnd))					// Are We Able To Destroy The Window?
	{
		mWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(_WIN_CLASS_NAME, mInstance))			// Are We Able To Unregister Class
	{
		mInstance=NULL;									// Set hInstance To NULL
	}
}

bool GLView::init(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	mInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= _WIN_CLASS_NAME;								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);// != DISP_CHANGE_SUCCESSFUL))

	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	wchar_t wtile[20];
	mbstowcs( &wtile[0], title, strlen(title)+1 );
	// Create The Window
	if (!(mWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								_WIN_CLASS_NAME,							// Class Name
								wtile,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								mInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(mDC=GetDC(mWnd)))							// Did We Get A Device Context?
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(mDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(mDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	if (!(mRC=wglCreateContext(mDC)))				// Are We Able To Get A Rendering Context?
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(mDC,mRC))					// Try To Activate The Rendering Context
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	ShowWindow(mWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(mWnd);						// Slightly Higher Priority
	SetFocus(mWnd);									// Sets Keyboard Focus To The Window
//	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		killGLWindow();								// Reset The Display
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

bool GLView::processEvents()
{
	if (mShutDown)
	{
		killGLWindow();
		return false;
	}

	MSG		msg;									// Windows Message Structure
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
	{
		if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
		{
			mShutDown=TRUE;							// If So done=TRUE
		}
		else									// If Not, Deal With Window Messages
		{
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
	return true;
}

void GLView::end()
{
	SwapBuffers(mDC);
}
