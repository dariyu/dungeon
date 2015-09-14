#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "d3d10_1.h"
#include "Window.h"
#include "common.h"
#include "Application.h"
#include "BlueScreen.h"
#include "Logger.h"
using namespace engine;

INT WINAPI WinMain(HINSTANCE instanse, HINSTANCE, LPSTR, INT){
	BlueScreen scene;
	Application *application = new Application();
	application->getWindow()->setInstance(instanse);
	application->getWindow()->setScene(&scene);
	application->getWindow()->setWidth(800);
	application->getWindow()->setHeight(800);
	application->Run();
	Logger::Release();
	delete application;
	return 0;
}