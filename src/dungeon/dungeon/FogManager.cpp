#include "FogManager.h"
#include "common.h"

namespace engine{
	const _ FogManager::max_fog_number = 64;
	FogManager::FogManager(Effect *effect){
		this->fog_number = 0;
		this->need_update_fog_array = true;
		this->need_sync = true;
		this->counter = 0;
		this->main_effect = effect;
	}
	_ FogManager::addFog(Fog input){
		this->fogs[this->counter] = input;
		this->fog_mask[this->counter] = true;
		this->fog_number++;
		this->need_sync = true;
		this->need_update_fog_array = true;
		return this->counter++;
	}
	void FogManager::updateFogType(_ id, FogType type){
		this->fogs[id].type = type;
		this->need_sync = true;
		this->need_update_fog_array = true;
	}
	void FogManager::disableFog(_ id){
		this->fog_mask[id] = false;
		this->need_sync = true;
	}
	void FogManager::enableFog(_ id){
		this->fog_mask[id] = false;
		this->need_sync = true;
	}
	void FogManager::removeFog(_ id){
		this->fogs.erase(id);
		this->fog_mask.erase(id);
		this->fog_number--;
		this->need_sync = true;
		this->need_update_fog_array = true;
	}
	void FogManager::prepareGPU(){
		if(this->need_sync){
			if(this->need_update_fog_array){
				_effect_variable fog_array = this->main_effect->getVariable("fogs");
				_effect_variable fog_count = this->main_effect->getVariable("fog_count");
				fog_count->AsScalar()->SetInt((int)this->fog_number);
				ID3D10EffectType *array_type = fog_array->GetType();
				D3D10_EFFECT_TYPE_DESC typeDesc;
				array_type->GetDesc(&typeDesc);
				for(_ i = 0; i < this->fog_number; ++i){
					ID3D10EffectVariable* current = fog_array->GetElement((UINT)i);
					_effect_vector color = current->GetMemberByName("color")->AsVector();
					_effect_vector scissor_plane = current->GetMemberByName("scissor_plane")->AsVector();
					_effect_scalar argument = current->GetMemberByName("argument")->AsScalar();
					_effect_scalar type = current->GetMemberByName("type")->AsScalar();
					type->SetInt(this->fogs[i].type);
					color->SetFloatVector(this->fogs[i].color);
					argument->SetFloat(this->fogs[i].argument);
					scissor_plane->SetFloatVector(this->fogs[i].scissor_plane);
				}
				this->need_sync = false;
			}
			_effect_variable fog_mask = this->main_effect->getVariable("fog_mask");
			ID3D10EffectType *array_type = fog_mask->GetType();
			D3D10_EFFECT_TYPE_DESC typeDesc;
			array_type->GetDesc(&typeDesc);
			for(_ i = 0; i < this->fog_number; ++i){
				ID3D10EffectVariable* current = fog_mask->GetElement((UINT)i);
				current->AsScalar()->SetBool(this->fog_mask[i]);
			}
		}
	}
	void FogManager::activate(){
		this->need_sync = true;
	}
}