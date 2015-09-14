#include "Application.h"
#include "Window.h"
#include "ThreadManager.h"

namespace engine{
	Application::Application(){
		this->window = new Window();
		this->thread_manager = new ThreadManager();
		this->thread_manager->setApplication(this);
		this->key_checker_thread = 0;
	}
	Window *Application::getWindow(){
		return this->window;
	}
	void Application::key_checker(ThreadManager *input){
		for(;;){
			input->getApplication()->window->keyChecker();
			Sleep(10);
		}
	}
	void Application::Run(){
		this->key_checker_thread = this->thread_manager->createThread(Application::key_checker);
		this->thread_manager->resumeThread(this->key_checker_thread);
		this->window->run();
		this->thread_manager->suspendThread(this->key_checker_thread);
	}
	Application::~Application(){
		delete this->window;
		delete this->thread_manager;
	}
}