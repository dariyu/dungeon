#ifndef __artist
#define __artist
#include "d3d10_1.h"
#include "common.h"
#include "d3dx10.h"
#include "TextureManager.h"
#include <map>
#include <vector>

namespace engine{
	class Effect;
	class VertexBuffer;
	class TextureManager;
	class Artist;
	class DepthBuffer;

	class Artist {
	public:
		Artist();
		~Artist();
		void createDepthBuffer(_ texture_id);
		void setCurrentDevice(_device input);
		void setSwapChain(_swap_chain input);
		void setMainRenderTargetView(_render_target_view input);
		_shader_resource_view getDepthBufferShaderResourceView(_ id);
		void setDisplayAsRenderTarget();
		void setTextureAsRenderTarget(_ texture_id);
		void prepareTextureRenderView(_ texture_id);
		void applyEffect(_ effect_number, _ buffer_number);
		void release();
		void renderToTexture();
		TextureManager *getTextureManager();
		_swap_chain getSwapChain();
		_device getDevice();
		_render_target_view getCurrentRenderTargetView();
		void buildDepthBuffer();
		_depth_stencil_view getDepthStencilView();
		_depth_stencil_view getActiveDepthStencilView();
		_depth_stencil_view getSecondaryDepthStencilView();
		_shader_resource_view getActiveDepthBufferShaderResourceView();
		inline _ getWidth() { return this->width; }
		inline _ getHeight() { return this->height; }
		inline void setWidth(_ input) { this->width = input; }
		inline void setHeight(_ input) { this->height = input; }
		inline void setBufferWidth(_ input) { this->buffer_width = input; }
		inline void setBufferHeight(_ input) { this->buffer_height = input; }
		inline _ getBufferWidth() { return this->buffer_width; }
		inline _ getBufferHeight() { return this->buffer_height; }
	private:
		_ width, height, buffer_height, buffer_width;
		_device device;
		DepthBuffer *active_depth_buffer;
		DepthBuffer *depth_buffer;
		DepthBuffer *secondary_depth_buffer;
		TextureManager *texture_manager;
		_swap_chain swap_chain;
		_render_target_view main_render_target_view;
		_render_target_view current_render_target_view;
		std::map <_,  _render_target_view > texture_target_views;
		std::map <_, DepthBuffer* > texture_depth_buffers;
	};

	class DepthBuffer {
	public:
		DepthBuffer(Artist *parent);
		void release();
		void fill(_ width, _ height);
		_depth_stencil_view getDepthStencilView();
		_shader_resource_view getShaderResourceView();
	private:
		_texture2d buffer;
		_depth_stencil_view depth_stencil_view;
		_shader_resource_view shader_resource_view;
		Artist *parent;
	};
}
#endif