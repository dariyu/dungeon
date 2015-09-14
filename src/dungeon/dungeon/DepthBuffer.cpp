#include "Artist.h"
#include "common.h"
#include "d3d10_1.h"
#include "d3dx10.h"

namespace engine{
	DepthBuffer::DepthBuffer(Artist *parent){
		this->buffer = _n;
		this->depth_stencil_view = _n;
		this->parent = parent;
		this->shader_resource_view = _n;
	}
	/*
		Not supported in DX 10.0 (may it will work under the DX 10.1)
	*/
	_shader_resource_view DepthBuffer::getShaderResourceView(){
		if(this->shader_resource_view != _n)
			return this->shader_resource_view;
		_shader_resource_view result;
		D3D10_SHADER_RESOURCE_VIEW_DESC resource_description;
		resource_description.Format = DXGI_FORMAT_D32_FLOAT;
		resource_description.Texture2D.MostDetailedMip = 0;
		resource_description.Texture2D.MipLevels = 1;
		resource_description.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		this->parent->getDevice()->CreateShaderResourceView(this->buffer, &resource_description, &this->shader_resource_view);
		return this->shader_resource_view;
	}
	void DepthBuffer::fill(_ width, _ height){
		HRESULT hr = S_OK;
		D3D10_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D10_USAGE_DEFAULT;
		descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = this->parent->getDevice()->CreateTexture2D(&descDepth, NULL, &this->buffer);
		if(FAILED(hr))
			return;
		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = this->parent->getDevice()->CreateDepthStencilView(this->buffer, &descDSV, &this->depth_stencil_view);
		if(FAILED(hr))
			return;
	}
	_depth_stencil_view DepthBuffer::getDepthStencilView(){
		return this->depth_stencil_view;
	}
	void DepthBuffer::release(){
		if(!this->buffer != _n){
			this->buffer->Release();
		}
		if(!this->depth_stencil_view != _n){
			this->depth_stencil_view->Release();
		}
	}
}