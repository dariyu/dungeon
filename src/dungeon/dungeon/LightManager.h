#ifndef __light_manager
#define __light_manager
#include "Effect.h"
#include "common.h"
#include <map>

#define _light_manager_max_light_number 128

namespace engine{
	struct Material{
		Material() : ambient(1.0f), diffuse(1.0f), specular(1.0f), specular_exp(40.0f), roughness(.3f), fresnel_reflectance(1.0f), metalness(.5f), smoothness(.5f), transparency(.0f) {
			this->ambient_color = _vector3(1.0f, 1.0f, 1.0f);
			this->diffuse_color = _vector3(1.0f, 1.0f, 1.0f);
			this->specular_color = _vector3(1.0f, 1.0f, 1.0f);
		}
		Material(_vector3 color) : ambient_color(color), diffuse_color(color), specular_color(color), ambient(1.0f), diffuse(1.0f), specular(1.0f), specular_exp(40.0f), roughness(.3f),  fresnel_reflectance(1.0f), metalness(.5f), smoothness(.5f), transparency(.0f) {}
		/*
			Commonly-used components.
		*/
		_vector3 ambient_color;
		_vector3 diffuse_color;
		_vector3 specular_color;
		float ambient;
		float diffuse;
		float specular;
		/*
			For Phong model.
		*/
		float specular_exp;
		/*
			For Cook-Torrance model. Also fresnel_reflectance reused in Strauss model.
		*/
		float fresnel_reflectance;
		float roughness;
		/*
			For Strauss model.
		*/
		float metalness;
		float smoothness;
		float transparency;
	};
	/*
		Caution : must be synchronized with shader code
	*/
	

	/*
		To the first sight to the problem : order of fields is important and must be same as order of fields of shader structure.
		P.S. it's true sice I use SetRawValue method of effect variables.
	*/
	struct LightSource{
		enum LightSourceType{
			Spot = 0,
			Spherical = 1
		};

		enum LightningModel{
			Ambient = 0,
			Diffuse = 1,
			Specular = 2,
			CookTorrance = 3,
			Strauss = 4
		};
		enum Attenuation{
			None = 0,
			Linear = 1
		};
		LightSourceType type;
		LightningModel lightning_model;
		Attenuation attenuation;
		_vector3 color;
		_vector3 position;
		_vector3 direction;
		float intensity;
		bool shadow;
		float radius;
	};

	class LightManager{
	public:
		const static _ max_light_number;
		LightManager(Effect *effect);
		_ addLightSource(LightSource input);
		void updateLightningModel(_ id, LightSource::LightningModel lightning_model);
		void disableLightSource(_ id);
		void enableLightSource(_ id);
		void removeLightSource(_ id);
		void prepareGPU();
		void prepareGPUMaterial(Material material);
		void activate();
		void toggleShadows(_ id, bool alpha);
		~LightManager();
	protected:
		LightSource *getLightSourcesArray();
		bool *getLightSourceMask();
		LightSource *light_sources_array;
		bool *mask_array;
		bool need_sync;
		bool need_update_light_array;
		_ light_number;
		_ counter;
		std::map < _, LightSource > light_sources;
		std::map < _, bool > source_mask;
		Effect *main_effect;
	};
}
#endif