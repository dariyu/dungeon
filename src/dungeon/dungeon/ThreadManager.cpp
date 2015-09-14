#include "ThreadManager.h"
#include <windows.h>

namespace engine{
	ThreadManager::ThreadManager(){
		this->counter = 0;
		this->application = _n;
	}
	ThreadManager::~ThreadManager(){
	}
	_d __stdcall ThreadManager::thread_executor(void *data){
		ThreadFrame *frame = (ThreadFrame*)data;
		frame->funciton(frame->parent);
		WaitForSingleObject(frame->parent->getHandle(frame->id), INFINITE);
		frame->parent->remove_thread(frame->id);
		return 0;
	}
	_ ThreadManager::createThread(ThreadFunction input){
		_h thread_handle = _n;
		_d thread_id = 0;
		ThreadFrame *frame = new ThreadFrame();
		frame->funciton = input;
		frame->parent = this;
		frame->id = this->counter;
		thread_handle = ::CreateThread(NULL, 0, ThreadManager::thread_executor, (void*)frame, CREATE_SUSPENDED, &thread_id);
		this->thread_ids[this->counter] = thread_id;
		this->thread_handles[this->counter] = thread_handle;
		return this->counter++;
	}
	void ThreadManager::remove_thread(_ input){
		this->thread_handles.erase(input);
		this->thread_ids.erase(input);
	}
	_h ThreadManager::getHandle(_ input){
		return this->thread_handles[input];
	}
	_d ThreadManager::getIdentifier(_ input){
		return this->thread_ids[input];
	}
	void ThreadManager::suspendThread(_ input){
		::SuspendThread(this->thread_handles[input]);
	}
	void ThreadManager::resumeThread(_ input){
		::ResumeThread(this->thread_handles[input]);
	}
	void ThreadManager::setApplication(Application *input){
		this->application = input;
	}
	Application *ThreadManager::getApplication(){
		return this->application;
	}
}