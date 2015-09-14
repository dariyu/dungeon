#ifndef __texture_manager
#define __texture_manager

#include "Artist.h"
#include "common.h"
#include <map>

namespace engine{
	class Artist;
	class TextureManager{
	public:
		TextureManager(Artist *artist);
		~TextureManager();
		_ createRenderTexture();
		_ getTextureWidth(_ id);
		_ getTextureHeight(_ id);
		_ createNoiseTexture(_ size, bool normal_mapping);
		_ createDepthTexture(_ size);
		_texture2d getTexture(_ id);
		_texture3d getTexture3d(_ id);
		_shader_resource_view createShaderResourceFromTexture(_ id);
		DXGI_FORMAT getTextureFormat(_ id);
	protected:
		struct texture{
			enum texture_type{
				_1d,
				_2d,
				_3d
			};
			DXGI_FORMAT format;
			int type;
			_ width;
			_ height;
			_ depth;
			_texture2d data_2d;
			_texture3d data_3d;
			_shader_resource_view resource_view;
			texture() : width(0), height(0), depth(0), data_2d(_n), data_3d(_n), resource_view(_n), type(texture::_1d), format(DXGI_FORMAT_R32G32B32_FLOAT) {}
		};
		Artist *artist;
		_ counter;
		std::map<_, texture> textures;
	};
}

#endif