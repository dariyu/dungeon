#include "PostEffectManager.h"
#include "Effect.h"
#include "Vertices.h"
#include <map>

namespace engine{
	void PostEffectManager::setupSceneSize(_vector2 size){
		this->scene_size = size;
	}
	void PostEffectManager::setupFocusData(_vector3 data){
		this->focus_data = data;
	}
	void PostEffectManager::setupDepthImage(_shader_resource_view image){
		this->depth_image = image;
	}
	PostEffectManager::PostEffectManager(Artist *artist){
		this->artist = artist;
		this->counter = 0;
		this->depth_image = _n;
		this->main_effect = new Effect(artist);
		this->main_effect->loadFromFile(L"post_effect_processor.fx");
		this->main_technique = this->main_effect->prepareTechnique("Process", Effect::PostEffectLayout);
		this->dof_technique = this->main_effect->prepareTechnique("DOF", Effect::PostEffectLayout);
		this->build_screen_quad();
	}
	void PostEffectManager::build_screen_quad(){
		screen_vertex *quad_data = new screen_vertex[4];
		_ curr = 0;
		quad_data[curr].position = _vector4(-1.0f, 1.0f, .5f, 1.0f);
		quad_data[curr++].texture_coordinate = _vector2(.0f, .0f);
		quad_data[curr].position = _vector4(1.0f, 1.0f, .5f, 1.0f);
		quad_data[curr++].texture_coordinate = _vector2(1.0f, .0f);
		quad_data[curr].position = _vector4(-1.0f, -1.0f, .5f, 1.0f);
		quad_data[curr++].texture_coordinate = _vector2(.0f, 1.0f);
		quad_data[curr].position = _vector4(1.0f, -1.0f, .5f, 1.0f);
		quad_data[curr++].texture_coordinate = _vector2(1.0f, 1.0f);
		D3D10_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(screen_vertex) * 4;
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = quad_data;
		if(FAILED(this->artist->getDevice()->CreateBuffer(&bd, &InitData, &this->screen_quad)))
			return;
		delete quad_data;
	}
	_ PostEffectManager::addPostEffect(PostEffect effect){
		this->post_effects[this->counter] = effect;
		return this->counter++;
	}
	void PostEffectManager::clearPostEffects(){
		this->post_effects.clear();
	}
	void PostEffectManager::activate(){
		this->main_effect->activateTechniqueLayout(this->main_technique);
		UINT stride = sizeof(screen_vertex);
		UINT offset = 0;
		this->artist->getDevice()->IASetVertexBuffers(0, 1, &this->screen_quad, &stride, &offset);
	}
	void PostEffectManager::setupImage(_ id){
		this->screen_image = id;
	}
	void PostEffectManager::prepareGPU(){
		_shader_resource_variable image = this->main_effect->getVariable("image")->AsShaderResource();
		_shader_resource_variable depth_image = this->main_effect->getVariable("depth_image")->AsShaderResource();
		image->SetResource(this->artist->getTextureManager()->createShaderResourceFromTexture(this->screen_image));
		if(this->depth_image != _n){
			depth_image->SetResource(this->depth_image);
		}
		_effect_variable post_effect_count = this->main_effect->getVariable("post_effect_count");
		_effect_variable focus_data = this->main_effect->getVariable("focus_data");
		_effect_variable scene_size = this->main_effect->getVariable("scene_size");
		_effect_variable post_effects = this->main_effect->getVariable("post_effects");
		focus_data->AsVector()->SetFloatVector(this->focus_data);
		scene_size->AsVector()->SetFloatVector(this->scene_size);
		post_effect_count->AsScalar()->SetInt((int)this->post_effects.size());
		ID3D10EffectType *array_type = post_effects->GetType();
		D3D10_EFFECT_TYPE_DESC typeDesc;
		array_type->GetDesc(&typeDesc);
		_ curr = 0;
		mapfor(i, _, PostEffect, this->post_effects){
			ID3D10EffectVariable* current = post_effects->GetElement((int)(curr++));
			_effect_scalar type = current->GetMemberByName("type")->AsScalar();
			_effect_scalar alpha = current->GetMemberByName("alpha")->AsScalar();
			type->SetInt(i->second.type);
			alpha->SetFloat(i->second.alpha);
		}
	}
	void PostEffectManager::PostProcessEntireImage(){
		this->main_effect->applyTechnique(this->main_technique);
		this->artist->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		this->artist->getDevice()->Draw(4, 0);
	}
	void PostEffectManager::DOFEntireImage(){
		this->artist->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		this->main_effect->applyTechniquePass(this->dof_technique, 0);
		this->artist->getDevice()->Draw(4, 0);
		this->main_effect->applyTechniquePass(this->dof_technique, 1);
		this->artist->getDevice()->Draw(4, 0);
	}
	PostEffectManager::~PostEffectManager(){
		this->main_effect->release();
		delete this->main_effect;
	}
}