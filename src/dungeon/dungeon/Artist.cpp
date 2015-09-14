#include "Artist.h"
#include "common.h"
#include "TextureManager.h"
#include <map>

namespace engine{
	Artist::Artist(){
		this->main_render_target_view = _n;
		this->current_render_target_view = _n;
		this->swap_chain = _n;
		this->device = _n;
		this->texture_manager = new TextureManager(this);
		this->width = this->height = 0;
		this->buffer_width = this->buffer_height = 0;
		this->depth_buffer = _n;
		this->secondary_depth_buffer = _n;
		this->active_depth_buffer = _n;
	}
	Artist::~Artist(){
		if(this->texture_manager != _n)
			delete this->texture_manager;
	}
	void Artist::release(){
		this->main_render_target_view->Release();
		this->swap_chain->Release();
		this->device->Release();
		this->device = _n;
		this->swap_chain = _n;
		this->main_render_target_view = _n;
		if(this->texture_manager != _n)
			delete this->texture_manager;
		this->texture_manager = _n;
		if(this->depth_buffer != _n)
			this->depth_buffer->release();
		if(this->secondary_depth_buffer != _n)
			this->secondary_depth_buffer->release();
		delete this->depth_buffer;
		delete this->secondary_depth_buffer;
		this->depth_buffer = _n;
		this->secondary_depth_buffer = _n;
	}
	_shader_resource_view Artist::getActiveDepthBufferShaderResourceView(){
		return this->active_depth_buffer->getShaderResourceView();
	}
	_shader_resource_view Artist::getDepthBufferShaderResourceView(_ id){
		return this->texture_depth_buffers[id]->getShaderResourceView();
	}
	void Artist::setCurrentDevice(_device input){
		this->device = input;
	}
	void Artist::setSwapChain(_swap_chain input){
		this->swap_chain = input;
	}
	_device Artist::getDevice(){
		return this->device;
	}
	_swap_chain Artist::getSwapChain(){
		return this->swap_chain;
	}
	_render_target_view Artist::getCurrentRenderTargetView(){
		return this->current_render_target_view;
	}
	TextureManager *Artist::getTextureManager(){
		return this->texture_manager;
	}
	void Artist::setMainRenderTargetView(_render_target_view input){
		this->main_render_target_view = input;
	}
	void Artist::buildDepthBuffer(){
		this->depth_buffer = new DepthBuffer(this);
		this->depth_buffer->fill(this->buffer_width, this->buffer_height);
		this->device->OMSetRenderTargets(1, &this->main_render_target_view, this->depth_buffer->getDepthStencilView());
	}
	void Artist::setDisplayAsRenderTarget(){
		this->active_depth_buffer = this->depth_buffer;
		this->current_render_target_view = this->main_render_target_view;
		this->device->OMSetRenderTargets(1, &this->current_render_target_view, this->active_depth_buffer->getDepthStencilView());
		D3D10_VIEWPORT vp;
		vp.Width = this->buffer_width;
		vp.Height = this->buffer_height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		device->RSSetViewports(1, &vp);
	}
	_depth_stencil_view Artist::getDepthStencilView(){
		return this->depth_buffer->getDepthStencilView();
	}
	_depth_stencil_view Artist::getActiveDepthStencilView(){
		return this->active_depth_buffer->getDepthStencilView();
	}
	_depth_stencil_view Artist::getSecondaryDepthStencilView(){
		return this->secondary_depth_buffer->getDepthStencilView();
	}
	void Artist::setTextureAsRenderTarget(_ texture_id){
		this->active_depth_buffer = this->texture_depth_buffers[texture_id];
		this->current_render_target_view = this->texture_target_views[texture_id];
		this->device->OMSetRenderTargets(1, &this->current_render_target_view, this->active_depth_buffer->getDepthStencilView());
		D3D10_VIEWPORT viewport;
		viewport.Width = this->texture_manager->getTextureWidth(texture_id);
		viewport.Height = this->texture_manager->getTextureHeight(texture_id);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		this->device->RSSetViewports(1, &viewport);
	}
	/*
		Deprecated: it will be removed soon.
	*/
	void Artist::createDepthBuffer(_ texture_id){
		this->texture_depth_buffers[texture_id] = new DepthBuffer(this);
		this->texture_depth_buffers[texture_id]->fill(this->texture_manager->getTextureWidth(texture_id), this->texture_manager->getTextureHeight(texture_id));
	}
	void Artist::prepareTextureRenderView(_ texture_id){
		this->texture_depth_buffers[texture_id] = new DepthBuffer(this);
		this->texture_depth_buffers[texture_id]->fill(this->texture_manager->getTextureWidth(texture_id), this->texture_manager->getTextureHeight(texture_id));
		_render_target_view current = _n;
		D3D10_RENDER_TARGET_VIEW_DESC description;
		description.Format = this->texture_manager->getTextureFormat(texture_id);;
		description.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		description.Texture2DArray.MipSlice = 0;
		this->device->CreateRenderTargetView(this->texture_manager->getTexture(texture_id), &description, &current);
		this->texture_target_views[texture_id] = current;
	}
}