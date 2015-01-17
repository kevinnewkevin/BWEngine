#include "base/Application.h"
#include "GLView.h"
#include "base/GameObject.h"
#include <algorithm>
#include <thread>
#include "base/component/Camera.h"
#include "base/component/SpriteRenderer.h"

Application* Application::sm_pSharedApplication = nullptr;
static Camera* camera = nullptr;

Application::Application()
: _instance(nullptr)
, _accelTable(nullptr)
, _needQuit(false)
{
    _instance    = GetModuleHandle(nullptr);
    _animationInterval.QuadPart = 0;
	assert(sm_pSharedApplication == nullptr);
    sm_pSharedApplication = this;

	setAnimationInterval(1/60.0);

	_RootGameObject = new GameObject();
	camera = new Camera();

	_RootGameObject->addComponent(camera);

	auto obj = new GameObject();
	SpriteRenderer* sp = new SpriteRenderer();
	obj->addComponent(sp);
	_RootGameObject->addChild(obj);


}

Application::~Application()
{
    assert(this == sm_pSharedApplication);
    sm_pSharedApplication = nullptr;
}

void Application::exit()
{
	_needQuit = true;
}

void Application::glThread()
{
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nLast;
    LARGE_INTEGER nNow;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nLast);
	GLView openglView;
	openglView.init("test", 800, 600, 16, false);

	
	CheckGLError();

	while(!openglView.mShutDown)
    {
        QueryPerformanceCounter(&nNow);
        if (nNow.QuadPart - nLast.QuadPart > _animationInterval.QuadPart)
        {
            nLast.QuadPart = nNow.QuadPart - (nNow.QuadPart % _animationInterval.QuadPart);
            
			openglView.processEvents();

			_RootGameObject->Update(nNow.QuadPart - nLast.QuadPart);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			camera->OnGUI();
			openglView.end();
        }
        else
        {
            Sleep(1);
        }
    }
	exit();
}

void Application::logicThread()
{
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nLast;
    LARGE_INTEGER nNow;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nLast);
	while(!_needQuit)
    {
        QueryPerformanceCounter(&nNow);
        if (nNow.QuadPart - nLast.QuadPart > _animationInterval.QuadPart)
        {
            nLast.QuadPart = nNow.QuadPart - (nNow.QuadPart % _animationInterval.QuadPart);
            
        }
        else
        {
            Sleep(1);
        }
    }
}

int Application::run()
{
    initGLContextAttrs();

    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

	std::thread trd(std::bind(&Application::logicThread, this));
	trd.detach();


	// glthread is main thread
	glThread();
    return true;
}

void Application::setAnimationInterval(double interval)
{
    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    _animationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

Application* Application::getInstance()
{
    assert(sm_pSharedApplication);
    return sm_pSharedApplication;
}