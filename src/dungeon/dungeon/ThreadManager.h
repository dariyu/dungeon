#ifndef __thread_manager
#define __thread_manager

#include <windows.h>
#include <map>
#include "common.h"
#include "Application.h"

namespace engine{
	class Application;

	class ThreadManager{
	public:
		typedef void (__stdcall *ThreadFunction)(ThreadManager*);
		struct ThreadFrame{
			ThreadManager *parent;
			ThreadFunction funciton;
			_ id;
		};
		void setApplication(Application *input);
		Application *getApplication();
		ThreadManager();
		~ThreadManager();
		_ createThread(ThreadFunction input);
		void suspendThread(_ input);
		void resumeThread(_ input);
		_h getHandle(_ input);
		_d getIdentifier(_ input);
	private:
		Application *application;
		static _d __stdcall thread_executor(void *data);
		void remove_thread(_ input);
		_ counter;
		std::map<_, _h> thread_handles;
		std::map<_, _d> thread_ids;
	};
}
#endif