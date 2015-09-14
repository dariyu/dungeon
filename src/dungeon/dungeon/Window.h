#ifndef __window
#define __window

#include <windows.h>
#include "common.h"
#include "Artist.h"
#include "Scene.h"

namespace engine {
	class Scene;

	class Window{
	public:
		Window();
		void setInstance(_hi input);
		void setWidth(_ input) { this->width = input; }
		void setHeight(_ input) { this->height = input; }
		void run();
		void setScene(Scene *input);
		_ulong checkKey(_ub input);
		~Window();
		void keyChecker();
		_ getCurrentPressedKey();
	private:
		_ width, height;
		_ current_pressed_key;
		static Window *current_window;
		_hi instance;
		_ulong keys[256];
		Artist *artist;
		Scene *scene;
		bool done;
		static LRESULT WINAPI message_processor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}

#endif