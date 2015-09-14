#ifndef __application
#define __application

#include <map>
#include "Window.h"
#include "ThreadManager.h"

namespace engine{
	class ThreadManager;

	class Application{
	public:
		Application();
		~Application();
		void Run();
		Window *getWindow();
	private:
		ThreadManager *thread_manager;
		_ key_checker_thread;
		static void _stdcall Application::key_checker(ThreadManager *input);
		Window *window;
	};
}

#endif