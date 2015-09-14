#include <windows.h>
#include "Window.h"
#include "Scene.h"
#include "common.h"
#include "d3d10_1.h"

namespace engine{
	Window *Window::current_window = _n;

	_ulong Window::checkKey(_ub input){
		return GetAsyncKeyState(input);
	}

	void Window::keyChecker(){
		this->scene->key_checker(this);
	}

	LRESULT WINAPI Window::message_processor(_hw hWnd, _uint msg, _wparam wParam, _lparam lParam){
		switch(msg){
			case WM_DESTROY:
				PostQuitMessage(0);
				Window::current_window->done = true;
				return 0;
			case WM_KEYUP:
				Window::current_window->keys[wParam] = 0;
				Window::current_window->current_pressed_key = wParam;
				Window::current_window->scene->key_checker(Window::current_window);
				Window::current_window->current_pressed_key = 0;
				break;
			case WM_KEYDOWN:
				Window::current_window->keys[wParam] = GetTickCount64();
				break;
			case WM_MOUSEMOVE:
				Window::current_window->scene->mouse_checker(wParam, LOWORD(lParam), HIWORD(lParam));
				break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	_ Window::getCurrentPressedKey(){
		return this->current_pressed_key;
	}

	Window::Window(){
		this->instance = _n;
		this->done = true;
		Window::current_window = this;
		this->artist = new Artist();
		this->scene = _n;
		this->width = this->height = 100;
	}

	Window::~Window(){
	}

	void Window::setInstance(_hi input){
		this->instance = input;
	}

	void Window::run(){
		this->done = false;
		WNDCLASSEX window_class = { sizeof(WNDCLASSEX), CS_CLASSDC, Window::message_processor,
									0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Dungeon", NULL };
		RegisterClassEx(&window_class);
		_hw hWnd = CreateWindow(L"Dungeon", L"Dungeon", WS_OVERLAPPEDWINDOW, 
									100, 100, (int)this->width, (int)this->height,	GetDesktopWindow(), NULL, window_class.hInstance, NULL);
		ShowWindow(hWnd, SW_SHOW);
		DXGI_SWAP_CHAIN_DESC sd;
		HRESULT result = S_OK;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = (UINT)this->width;
		sd.BufferDesc.Height = (UINT)this->height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		_swap_chain swap_chain = NULL;
		_device device = NULL;
		/*IDXGIFactory* factory = NULL;
		IDXGIAdapter* adapter = NULL;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));
		result = factory->MakeWindowAssociation(NULL, 0);
		result = factory->EnumAdapters( 0, &adapter );*/
		_d flags = 0;
		IDXGIAdapter* selectedAdapter = NULL;
		D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;
#ifdef _DEBUG
		flags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
#ifdef _PERFHUD
		IDXGIFactory *pDXGIFactory;
		HRESULT hRes;
		hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
		UINT nAdapter = 0;
		IDXGIAdapter* adapter = NULL;
		while(pDXGIFactory->EnumAdapters(nAdapter, &adapter) != DXGI_ERROR_NOT_FOUND){
			if(adapter){
				DXGI_ADAPTER_DESC adaptDesc;
				if (SUCCEEDED(adapter->GetDesc(&adaptDesc))){
					const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;
						if(nAdapter == 0 || isPerfHUD)
							selectedAdapter = adapter;
					if(isPerfHUD)
						driverType = D3D10_DRIVER_TYPE_REFERENCE;
				}
			}
			++nAdapter;
		}
#endif
#ifdef _D3D10_1
		result = D3D10CreateDeviceAndSwapChain1(selectedAdapter, driverType, 
					NULL, flags, D3D10_FEATURE_LEVEL_10_0, D3D10_1_SDK_VERSION, &sd, &swap_chain, &device);
#endif
#ifdef _D3D10
		result = D3D10CreateDeviceAndSwapChain(selectedAdapter, driverType, 
			NULL, flags, D3D10_SDK_VERSION, &sd, &swap_chain, &device);
#endif
		if(FAILED(result)){
			return;
		}
		_texture2d backBuffer = NULL;
		_render_target_view render_target_view = NULL;
		if(FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBuffer)))
			return;
		result = device->CreateRenderTargetView(backBuffer, NULL, &render_target_view);
		backBuffer->Release();
		this->artist->setMainRenderTargetView(render_target_view);
		if(FAILED(result))
			return;
		MSG message;
		this->artist->setBufferWidth(this->width);
		this->artist->setBufferHeight(this->height);
		this->artist->setWidth(this->width);
		this->artist->setHeight(this->height);
		this->artist->setCurrentDevice(device);
		this->scene->setup(this->artist);
		this->artist->buildDepthBuffer();
		this->artist->setDisplayAsRenderTarget();
		this->artist->setSwapChain(swap_chain);
		while(!this->done){
			if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)){
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else{
				if(this->scene != _n){
					this->scene->render();
				}
			}
		}
		this->scene->release();
		this->artist->release();
	}
	void Window::setScene(Scene *input){
		this->scene = input;
	}
}