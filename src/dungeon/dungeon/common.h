#ifndef __common
#define __common

#include <windows.h>
#include "d3d10_1.h"
#include "d3dx10.h"

#define _D3D10
#define _PERFHUD
#define _n NULL

namespace engine{
	typedef HANDLE _h;
	typedef HINSTANCE _hi;
	typedef HWND _hw;
	typedef DWORD _d;
	typedef size_t _;
	typedef UINT _uint;
	typedef LPARAM _lparam;
	typedef WPARAM _wparam;
	typedef unsigned char _ub;
	typedef ULONGLONG _ulong;
	typedef unsigned char _uint8;
}

#ifdef _D3D10_1
#define _device ID3D10Device1*
#endif
#ifdef _D3D10
#define _device ID3D10Device*
#endif
#define _device10 ID3D10Device*
#define _swap_chain IDXGISwapChain*
#define _render_target_view ID3D10RenderTargetView*
#define _effect ID3D10Effect*
#define _buffer ID3D10Buffer*
#define _technique ID3D10EffectTechnique*
#define _vector3 D3DXVECTOR3
#define _vector4 D3DXVECTOR4
#define _matrix D3DXMATRIX
#define _effect_matrix ID3D10EffectMatrixVariable*
#define _effect_vector ID3D10EffectVectorVariable*
#define _effect_variable ID3D10EffectVariable*
#define _effect_scalar ID3D10EffectScalarVariable*
#define _texture2d ID3D10Texture2D*
#define _texture3d ID3D10Texture3D*
#define _depth_stencil_view ID3D10DepthStencilView*
#define _shader_resource_view ID3D10ShaderResourceView*
#define _input_layout ID3D10InputLayout*
#define _vector2 D3DXVECTOR2
#define _shader_resource_variable ID3D10EffectShaderResourceVariable*

#define mapfor(iterator_name, key_type, value_type, container_name) \
	for(std::map < key_type, value_type > :: iterator iterator_name = container_name.begin(); iterator_name != container_name.end(); iterator_name++)
/*
	For some historical reasons I will name this macro as eplfor
*/
#define eplfor(variable_name, upper_limit) for(_ variable_name = 0; variable_name < upper_limit; variable_name++)

void common_merge_sort(void *what, int number_elements, int size_of_element, int (*compare)(const void *a, const void *b), void *buffer);
#endif