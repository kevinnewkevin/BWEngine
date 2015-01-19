#ifndef __APPLICATION_WIN32_H__
#define __APPLICATION_WIN32_H__

#include <string>
#include <windows.h>
#include <assert.h>
#include <shellapi.h>

class GameObject;
class Application
{
public:

    Application();
    
    virtual ~Application();

    int run();
	void exit();

    static Application* getInstance();
	
	void glThread();
	void logicThread();
	
	virtual bool applicationDidFinishLaunching() {return true;}
	virtual void applicationDidEnterBackground() {}
	virtual void applicationWillEnterForeground() {}

	virtual void setAnimationInterval(double interval);

	virtual void initGLContextAttrs();

protected:
    HINSTANCE           _instance;
    HACCEL              _accelTable;
    LARGE_INTEGER       _animationInterval;
    std::string         _resourceRootPath;
    std::string         _startupScriptFilename;

	bool _needQuit;

	GameObject* _RootGameObject;
	
    static Application * sm_pSharedApplication;
};

#endif    // __CC_APPLICATION_WIN32_H__
