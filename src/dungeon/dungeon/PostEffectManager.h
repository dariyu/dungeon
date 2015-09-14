#ifndef __post_effect_manager
#define __post_effect_manager

#include "Artist.h"
#include "Effect.h"
#include <map>

namespace engine{
	enum PostEffectType{
		Monochrome = 0,
		Saturate = 1,
	};
	/*
		alpha is the first parameter for post effect
	*/
	struct PostEffect{
		int type;
		float alpha;
	};
	class PostEffectManager{
	public:
		PostEffectManager(Artist *artist);
		_ addPostEffect(PostEffect effect);
		void clearPostEffects();
		void setupSceneSize(_vector2 size);
		void setupFocusData(_vector3 data);
		void setupDepthImage(_shader_resource_view image);
		void prepareGPU();
		void activate();
		void setupImage(_ id);
		void apply();
		void PostProcessEntireImage();
		void DOFEntireImage();
		~PostEffectManager();
	protected:
		_vector2 scene_size;
		_vector3 focus_data;
		_shader_resource_view depth_image;
		void build_screen_quad();
		Effect *main_effect;
		Artist *artist;
		_buffer screen_quad;
		_ counter;
		_ screen_image;
		_ main_technique, dof_technique;
		std::map < _, PostEffect > post_effects;
	};
}
#endif