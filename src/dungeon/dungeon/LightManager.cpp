#include "LightManager.h"
#include "Effect.h"
#include "common.h"
#include <stdlib.h>
#include <map>

namespace engine{
	const _ LightManager::max_light_number = 128;
	void LightManager::toggleShadows(_ id, bool alpha){
		this->light_sources[id].shadow = alpha;
	}
	LightManager::LightManager(Effect *effect){
		this->need_sync = true;
		this->main_effect = effect;
		this->counter = 0;
		this->light_sources_array = _n;
		this->mask_array = _n;
		this->light_number = 0;
	}
	void LightManager::activate(){
		this->need_sync = true;
	}
	void LightManager::updateLightningModel(_ id, LightSource::LightningModel lightning_model){
		this->light_sources[id].lightning_model = lightning_model;
		this->need_update_light_array = true;
		this->need_sync = true;
	}
	_ LightManager::addLightSource(LightSource input){
		this->need_sync = true;
		this->need_update_light_array = true;
		this->light_sources[this->counter] = input;
		this->source_mask[this->counter] = true;
		this->light_number++;
		return this->counter++;
	}
	void LightManager::disableLightSource(_ id){
		this->need_sync = true;
		this->source_mask[id] = false;
	}
	void LightManager::enableLightSource(_ id){
		this->need_sync = true;
		this->source_mask[id] = true;
	}
	void LightManager::removeLightSource(_ id){
		this->need_sync = true;
		this->need_update_light_array = true;
		this->source_mask.erase(id);
		this->light_sources.erase(id);
		this->light_number--;
	}
	LightSource* LightManager::getLightSourcesArray(){
		this->light_sources_array = (LightSource*)realloc(this->light_sources_array, sizeof(LightSource) * this->light_number);
		_ curr = 0;
		mapfor(i, _, LightSource, this->light_sources){
			this->light_sources_array[curr++] = i->second;
		}
		return this->light_sources_array;
	}
	bool* LightManager::getLightSourceMask(){
		this->mask_array = (bool*)realloc(this->mask_array, sizeof(bool) * this->light_number);
		_ curr = 0;
		mapfor(i, _, bool, this->source_mask){
			this->mask_array[curr++] = i->second;
		}
		return this->mask_array;
	}
	void LightManager::prepareGPU(){
		if(this->need_sync){
			if(this->need_update_light_array){
				_effect_variable light_array = this->main_effect->getVariable("light_sources");
				_effect_variable light_count = this->main_effect->getVariable("light_count");
				light_count->AsScalar()->SetInt(this->light_number);
				ID3D10EffectType *array_type = light_array->GetType();
				D3D10_EFFECT_TYPE_DESC typeDesc;
				array_type->GetDesc(&typeDesc);
				this->light_sources_array = this->getLightSourcesArray();
				for(_ i = 0; i < this->light_number; ++i){
					ID3D10EffectVariable* current = light_array->GetElement(i);
					_effect_vector color = current->GetMemberByName("color")->AsVector();
					_effect_vector position = current->GetMemberByName("position")->AsVector();
					_effect_vector direction = current->GetMemberByName("direction")->AsVector();
					_effect_scalar intensity = current->GetMemberByName("intensity")->AsScalar();
					_effect_scalar radius = current->GetMemberByName("radius")->AsScalar();
					_effect_scalar lightning_model = current->GetMemberByName("lightning_model")->AsScalar();
					_effect_scalar type = current->GetMemberByName("type")->AsScalar();
					_effect_scalar attenuation = current->GetMemberByName("attenuation")->AsScalar();
					lightning_model->SetInt(this->light_sources_array[i].lightning_model);
					attenuation->SetInt(this->light_sources_array[i].attenuation);
					type->SetInt(this->light_sources_array[i].type);
					intensity->SetFloat(this->light_sources_array[i].intensity);
					direction->SetFloatVector(this->light_sources_array[i].direction);
					position->SetFloatVector(this->light_sources_array[i].position);
					color->SetFloatVector(this->light_sources_array[i].color);
					radius->SetFloat(this->light_sources_array[i].radius);
				}

				this->need_sync = false;
			}
			this->mask_array = this->getLightSourceMask();
			_effect_variable source_mask = this->main_effect->getVariable("light_sources_mask");
			ID3D10EffectType *array_type = source_mask->GetType();
			D3D10_EFFECT_TYPE_DESC typeDesc;
			array_type->GetDesc(&typeDesc);
			for(_ i = 0; i < this->light_number; ++i){
				ID3D10EffectVariable* current = source_mask->GetElement(i);
				current->AsScalar()->SetBool(this->mask_array[i]);
			}
		}
	}
	LightManager::~LightManager(){
		if(this->mask_array != _n){
			free(this->mask_array);
		}
		if(this->light_sources_array != _n){
			free(this->light_sources_array);
		}
	}
	void LightManager::prepareGPUMaterial(Material material){
		_effect_variable current = this->main_effect->getVariable("ObjectMaterial");
		_effect_vector ambient_color = current->GetMemberByName("ambient_color")->AsVector();
		_effect_vector diffuse_color = current->GetMemberByName("diffuse_color")->AsVector();
		_effect_vector specular_color = current->GetMemberByName("specular_color")->AsVector();
		_effect_scalar ambient = current->GetMemberByName("ambient")->AsScalar();
		_effect_scalar diffuse = current->GetMemberByName("diffuse")->AsScalar();
		_effect_scalar specular = current->GetMemberByName("specular")->AsScalar();
		_effect_scalar specular_exp = current->GetMemberByName("specular_exp")->AsScalar();
		_effect_scalar fresnel_reflectance = current->GetMemberByName("fresnel_reflectance")->AsScalar();
		_effect_scalar roughness = current->GetMemberByName("roughness")->AsScalar();
		_effect_scalar smoothness = current->GetMemberByName("smoothness")->AsScalar();
		_effect_scalar metalness = current->GetMemberByName("metalness")->AsScalar();
		_effect_scalar transparency = current->GetMemberByName("transparency")->AsScalar();
		ambient_color->SetFloatVector(material.ambient_color);
		diffuse_color->SetFloatVector(material.diffuse_color);
		specular_color->SetFloatVector(material.specular_color);
		ambient->SetFloat(material.ambient);
		diffuse->SetFloat(material.diffuse);
		specular->SetFloat(material.specular);
		specular_exp->SetFloat(material.specular_exp);
		fresnel_reflectance->SetFloat(material.fresnel_reflectance);
		roughness->SetFloat(material.roughness);
		smoothness->SetFloat(material.smoothness);
		metalness->SetFloat(material.metalness);
		transparency->SetFloat(material.transparency);
	}
}