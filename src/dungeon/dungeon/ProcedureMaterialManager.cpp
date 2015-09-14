#include "ProcedureMaterialManager.h"
#include "Effect.h"
#include "Artist.h"

namespace engine{
	ProcedureMaterialManager::ProcedureMaterialManager(Artist *artist, Effect *effect){
		this->enabled = false;
		this->need_sync = true;
		this->main_effect = effect;
		this->noise_texture = 0;
		this->artist = artist;
		this->noise_texture_loaded = false;
	}
	void ProcedureMaterialManager::setMaterial(ProcedureMaterial alpha){
		this->current = alpha;
		this->need_sync = true;
	}
	void ProcedureMaterialManager::toggleProcessingProcedureMaterials(bool alpha){
		this->enabled = alpha;
		this->need_sync = true;
	}
	void ProcedureMaterialManager::setNoiseTexture(_ id){
		this->noise_texture = id;
	}
	void ProcedureMaterialManager::PrepareGPU(){
		if(this->need_sync){
			_effect_variable state = this->main_effect->getVariable("procedure_material_enable");
			state->AsScalar()->SetBool(this->enabled);
			if(this->enabled){
				_effect_variable current = this->main_effect->getVariable("ObjectProcedureMaterial");
				_effect_scalar type = current->GetMemberByName("type")->AsScalar();
				_effect_scalar normal_mapping = current->GetMemberByName("normal_mapping")->AsScalar();
				type->SetInt(this->current.type);
				normal_mapping->SetBool(this->current.normal_mapping);
			}
		}
		if(!this->noise_texture_loaded){
			_shader_resource_variable current = this->main_effect->getVariable("noise_texture")->AsShaderResource();
			current->SetResource(this->artist->getTextureManager()->createShaderResourceFromTexture(this->noise_texture));
			this->noise_texture_loaded = true;
		}
	}
}