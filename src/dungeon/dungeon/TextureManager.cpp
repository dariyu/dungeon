#include "TextureManager.h"
#include "GraphicExpert.h"
#include "Artist.h"
#include "common.h"

namespace engine{
	TextureManager::TextureManager(Artist *artist){
		this->artist = artist;
		this->counter = 0;
	}
	_ TextureManager::createRenderTexture(){
		D3D10_TEXTURE2D_DESC texture_description;
		texture current;
		ZeroMemory(&texture_description, sizeof(texture_description));
		texture_description.Width = this->artist->getBufferWidth();
		texture_description.Height = this->artist->getBufferHeight();
		texture_description.MipLevels = 1;
		texture_description.ArraySize = 1;
		current.format = texture_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_description.SampleDesc.Count	= 1;
		texture_description.Usage = D3D10_USAGE_DEFAULT;
		texture_description.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		_texture2d result;
		this->artist->getDevice()->CreateTexture2D(&texture_description, _n, &result);
		current.width = texture_description.Width;
		current.height = texture_description.Height;
		current.data_2d = result;
		current.type = texture::_2d;
		this->textures[this->counter] = current;
		return this->counter++;
	}
	_ TextureManager::createDepthTexture(_ size){
		D3D10_TEXTURE2D_DESC texture_description;
		texture current;
		ZeroMemory(&texture_description, sizeof(texture_description));
		texture_description.Width = size;
		texture_description.Height = size;
		texture_description.MipLevels = 1;
		texture_description.ArraySize = 1;
		current.format = texture_description.Format = DXGI_FORMAT_R32_FLOAT;
		texture_description.SampleDesc.Count = 1;
		texture_description.Usage = D3D10_USAGE_DEFAULT;
		texture_description.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		_texture2d result;
		this->artist->getDevice()->CreateTexture2D(&texture_description, _n, &result);
		current.width = texture_description.Width;
		current.height = texture_description.Height;
		current.data_2d = result;
		current.type = texture::_2d;
		this->textures[this->counter] = current;
		return this->counter++;
	}
	_ TextureManager::createNoiseTexture(_ size, bool normal_mapping){
		D3D10_TEXTURE3D_DESC texture_description;
		texture current;
		ZeroMemory(&texture_description, sizeof(texture_description));
		texture_description.Width = size;
		texture_description.Height = size;
		texture_description.Depth = size;
		texture_description.MipLevels = 1;
		texture_description.CPUAccessFlags = 0;
		if(normal_mapping){
			current.format = texture_description.Format = DXGI_FORMAT_R32_FLOAT;
		} else {
			current.format = texture_description.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		texture_description.Usage = D3D10_USAGE_IMMUTABLE;
		texture_description.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		_texture3d result;
		D3D10_SUBRESOURCE_DATA texture_data[1];
		memset(&texture_data, 0, sizeof(D3D10_SUBRESOURCE_DATA) * 1);
		float *noise_texture = GraphicExpert::createNoiseTexture(size, normal_mapping);
		texture_data[0].pSysMem = noise_texture;
		texture_data[0].SysMemPitch = sizeof(float) * size;
		texture_data[0].SysMemSlicePitch = sizeof(float) * size * size;
		if(normal_mapping){
			texture_data[0].SysMemPitch *= 3;
			texture_data[0].SysMemSlicePitch *= 3;
		}
		this->artist->getDevice()->CreateTexture3D(&texture_description, texture_data, &result);

		delete noise_texture;
		current.type = texture::_3d;
		current.width = texture_description.Width;
		current.height = texture_description.Height;
		current.depth = texture_description.Depth;
		current.data_3d = result;
		this->textures[this->counter] = current;
		return this->counter++;
	}
	_shader_resource_view TextureManager::createShaderResourceFromTexture(_ id){
		if(this->textures[id].resource_view != _n)
			return this->textures[id].resource_view;
		_shader_resource_view result;
		D3D10_SHADER_RESOURCE_VIEW_DESC resource_description;
		resource_description.Format = this->textures[id].format;
		resource_description.Texture2D.MostDetailedMip = 0;
		resource_description.Texture2D.MipLevels = 1;
		switch(this->textures[id].type){
			case texture::_2d:
				resource_description.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
				break;
			case texture::_3d:
				resource_description.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
				break;
		}
		switch(this->textures[id].type){
			case texture::_2d:
				this->artist->getDevice()->CreateShaderResourceView(this->textures[id].data_2d, &resource_description, &result);
				break;
			case texture::_3d:
				this->artist->getDevice()->CreateShaderResourceView(this->textures[id].data_3d, &resource_description, &result);
				break;
		}
		this->textures[id].resource_view = result;
		return result;
	}
	DXGI_FORMAT TextureManager::getTextureFormat(_ id){
		return this->textures[id].format;
	}
	TextureManager::~TextureManager(){
		mapfor(i, _, texture, this->textures){
			if(i->second.resource_view != _n){
				i->second.resource_view->Release();
			}
			switch(i->second.type){
				case texture::_2d:
					i->second.data_2d->Release();
					break;
				case texture::_3d:
					i->second.data_3d->Release();
					break;
			}
		}
	}
	_texture2d TextureManager::getTexture(_ id){
		return this->textures[id].data_2d;
	}
	_texture3d TextureManager::getTexture3d(_ id){
		return this->textures[id].data_3d;
	}
	_ TextureManager::getTextureHeight(_ id){
		return this->textures[id].height;
	}
	_ TextureManager::getTextureWidth(_ id){
		return this->textures[id].width;
	}
}