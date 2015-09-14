#include "Scene.h"
#include "Artist.h"
#include "d3d10_1.h"
#include "common.h"
#include "Primitives.h"
#include "Landscape.h"
#include "Effect.h"
#include "LightManager.h"
#include "BlueScreen.h"
#include "Logger.h"
#include "PostEffectManager.h"
#include "ProcedureMaterialManager.h"
#include "SimplexNoise.h"
#include "FogManager.h"
#include "BillboardManager.h"

namespace engine{
	void BlueScreen::prepare_shadows(Camera *current){
		HRESULT checker = S_OK;
		ID3D10EffectMatrixVariable* FullShadowTransformation = NULL;
		ID3D10EffectMatrixVariable* CoordinateTransformation = NULL;
		FullShadowTransformation = this->main_effect->getVariable("FullShadowTransformation")->AsMatrix();
		CoordinateTransformation = this->main_effect->getVariable("TextureTransformation")->AsMatrix();
		D3DXMATRIX shadow_camera_matrix;
		D3DXMATRIX shadow_projection_matrix;
		shadow_camera_matrix = this->shadow_source.calculateTransformation();
		D3DXMatrixPerspectiveFovLH(&shadow_projection_matrix, (float)D3DX_PI / 4.0f, 1.0f, .00001f, 1024.0f);
		float fOffset = 0.5f;
		float fBias = .0f;
		D3DXMATRIX texScaleBiasMat = D3DXMATRIX(.5f,		.0f,		.0f,	.0f,
												.0f,		-.5f,		.0f,	.0f,
												.0f,		.0f,		1.0f,	.0f,
												fOffset,	fOffset,	fBias,	1.0f);
		_matrix world_matrix;
		D3DXMatrixIdentity(&world_matrix);
		D3DXMatrixScaling(&world_matrix, .05f, .05f, .05f);
		D3DXMATRIX full_transformation;

		D3DXMATRIX TextureTransformation;
		full_transformation = shadow_camera_matrix * world_matrix * shadow_projection_matrix;
		D3DXMatrixMultiply(&TextureTransformation, &full_transformation, &texScaleBiasMat);

		full_transformation = shadow_camera_matrix * world_matrix * shadow_projection_matrix;
		checker = FullShadowTransformation->SetMatrix((float*)&full_transformation);
		checker = CoordinateTransformation->SetMatrix((float*)&TextureTransformation);
	}
	void BlueScreen::render_objects(Camera *camera, _ main_technigue, bool draw_transparent, bool draw_mirrors, float time){
		this->main_effect->getVariable("time")->AsScalar()->SetFloat(time);
		this->main_buffer->activate();
		this->main_effect->activateTechniqueLayout(main_technigue);
		this->light_manager->activate();
		this->light_manager->prepareGPU();
		this->fog_manager->activate();
		this->fog_manager->prepareGPU();
		this->procedure_material_manager->toggleProcessingProcedureMaterials(false);
		this->procedure_material_manager->PrepareGPU();
		this->prepare_shadows(camera);
		this->main_effect->getVariable("opaque")->AsScalar()->SetFloat(1.0f);
		_matrix camera_matrix = camera->calculateTransformation();
		_matrix object_position;
		_matrix temp_matrix;
		_matrix world_matrix;
		D3DXMatrixIdentity(&object_position);
		D3DXMatrixIdentity(&world_matrix);
		D3DXMatrixIdentity(&temp_matrix);
		D3DXMatrixScaling(&world_matrix, .001f, .001f, .001f);
		D3DXMatrixTranslation(&temp_matrix, .0f, -10.0f, .0f);
		D3DXMatrixMultiply(&object_position, &object_position, &temp_matrix);
		this->main_trasformation = camera_matrix * world_matrix * this->projection_matrix;
		_effect_matrix transformation = main_effect->getVariable("MainTransformation")->AsMatrix();
		_effect_matrix world_transformation = main_effect->getVariable("WorldTransformation")->AsMatrix();
		_effect_matrix position = main_effect->getVariable("ObjectPosition")->AsMatrix();
		_effect_vector camera_position = main_effect->getVariable("CameraPosition")->AsVector();
		_effect_vector camera_up = main_effect->getVariable("CameraUp")->AsVector();
		_effect_vector camera_right = main_effect->getVariable("CameraRight")->AsVector();
		_vector3 temp = camera->getPosition();
		camera_position->SetFloatVector((float*)temp);
		camera_up->SetFloatVector((float*)camera->getUp());
		camera_right->SetFloatVector((float*)camera->getRight());
		transformation->SetMatrix((float*)this->main_trasformation);
		position->SetMatrix((float*)object_position);
		world_transformation->SetMatrix((float*)world_matrix);
		this->light_manager->prepareGPUMaterial(this->landscape_material);
		this->main_effect->applyTechnique(main_technigue);
		this->landscape->draw();
		
		if(this->only_landscape){
			return;
		}
		D3DXMatrixTranslation(&object_position, 4.0f, 2.0f, .0f);
		D3DXMatrixRotationY(&temp_matrix, (float)time);
		object_position *= temp_matrix;
		position->SetMatrix((float*)object_position);
		this->light_manager->prepareGPUMaterial(this->cube_material);
		this->main_effect->applyTechnique(main_technigue);
		this->main_buffer->draw(this->cube);

		this->light_manager->prepareGPUMaterial(this->morph_material);
		eplfor(i, 10){
			float factor = 1.0f / ((float)i + 1.0f);
			D3DXMatrixRotationZ(&object_position, (float)time);
			D3DXMatrixScaling(&temp_matrix, factor, factor, factor);
			object_position *= temp_matrix;
			D3DXMatrixTranslation(&temp_matrix, 6.0f * factor * sinf(time + (float)i), 3.0f + (float)i, 10.0f * factor * cosf(time + (float)i));
			object_position *= temp_matrix;
			position->SetMatrix((float*)object_position);
			this->main_effect->applyTechnique(main_technigue);
			this->main_buffer->draw(this->shape);
		}

		D3DXMatrixTranslation(&object_position, .0f, 2.0f, .0f);
		D3DXMatrixRotationY(&temp_matrix, (float)angle);
		object_position *= temp_matrix;
		position->SetMatrix((float*)object_position);
		this->procedure_material_manager->toggleProcessingProcedureMaterials(this->enable_procedure_materials);
		this->light_manager->prepareGPUMaterial(this->cube_material);
		this->marble.normal_mapping = this->enable_normal_mapping;
		this->procedure_material_manager->setMaterial(this->marble);
		this->procedure_material_manager->PrepareGPU();
		this->main_effect->applyTechnique(main_technigue);
		this->main_buffer->draw(this->cube);

		this->procedure_material_manager->toggleProcessingProcedureMaterials(false);
		this->procedure_material_manager->PrepareGPU();


		if(draw_mirrors){
			_shader_resource_variable image = this->main_effect->getVariable("current_texture")->AsShaderResource();
			image->SetResource(this->artist->getTextureManager()->createShaderResourceFromTexture(this->mirror_texture));
			D3DXMatrixIdentity(&object_position);
			D3DXMatrixTranslation(&object_position, .0f, 2.0f, 10.0f);
			position->SetMatrix((float*)object_position);
			this->main_effect->activateTechniqueLayout(this->texturing_technique);
			this->texture_objects_buffer->activate();
			this->light_manager->disableLightSource(0);
			this->light_manager->prepareGPU();
			this->light_manager->prepareGPUMaterial(this->mirror_material);
			this->main_effect->applyTechnique(this->texturing_technique);
			this->texture_objects_buffer->draw(this->mirror);
		}

		this->light_manager->enableLightSource(0);
		this->main_effect->activateTechniqueLayout(this->normal_technique);
		this->main_buffer->activate();
		/*
			Drawing translucency geometry.
		*/

		if(draw_transparent){
			D3DXMatrixTranslation(&object_position, .0f, 2.0f, .0f);
			D3DXMatrixRotationX(&temp_matrix, (float)time);
			object_position *= temp_matrix;
			D3DXMatrixTranslation(&temp_matrix, .0f, 10.0f, .0f);
			object_position *= temp_matrix;
			position->SetMatrix((float*)object_position);
			this->main_effect->applyTechnique(this->transparent_technique);
			this->translucency_manager->setObjectTransformation(this->transparent_cube, object_position);
			this->translucency_manager->setCameraPosition(camera->getPosition());
			this->translucency_manager->sort();
			//this->translucency_manager->cuda_sort();
			this->translucency_manager->draw();
		}
		/*
			Drawing billboards
		*/

		if(main_technigue != building_depth_texture_technique){
			this->billboard_manager->setupCamera(camera->getPosition());
			this->billboard_manager->activate();
			this->billboard_manager->sort();
			this->billboard_manager->draw();

			this->particle_manager->setupCamera(camera->getPosition());
			this->particle_manager->activate();
			this->particle_manager->sort();
			this->particle_manager->draw();
		}
	}
	void BlueScreen::render(){
		float time = (float)GetTickCount() / 1024.0f;
		this->morph_material.diffuse_color = _vector3(1.0f, fabs(sinf(time)), .0f);
		this->morph_material.specular_color = _vector3(1.0f, fabs(sinf(time)), .0f);
		HRESULT checker = S_OK;
		float black[4] = {.0f, .0f, .0f, 1.0f};
		float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		this->artist->setTextureAsRenderTarget(this->depth_texture);
		this->artist->getDevice()->ClearRenderTargetView(this->artist->getCurrentRenderTargetView(), black);
		this->artist->getDevice()->ClearDepthStencilView(this->artist->getActiveDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f, 0);
		this->prepare_shadows(this->camera);
		this->render_objects(&this->shadow_source, this->building_depth_texture_technique, false, false, time);

		this->artist->setTextureAsRenderTarget(this->mirror_texture);
		this->artist->getDevice()->ClearRenderTargetView(this->artist->getCurrentRenderTargetView(), black);
		this->artist->getDevice()->ClearDepthStencilView(this->artist->getActiveDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f, 0);
		this->portal_camera.set(_vector3(.0f, .0f, 10.0f), _vector3(.0f, .0f, -1.0f), _vector3(.0f, 1.0f, .0f));

		_shader_resource_variable image = this->main_effect->getVariable("DepthMap")->AsShaderResource();
		checker = image->SetResource(this->artist->getTextureManager()->createShaderResourceFromTexture(this->depth_texture));

		if(this->shadows){
			this->render_objects(&this->portal_camera, this->shadow_rendering_techique, true, true, time);
		} else {
			this->render_objects(&this->portal_camera, this->normal_technique, true, true, time);
		}

		this->artist->setTextureAsRenderTarget(this->screen_texture);
		this->artist->getDevice()->ClearRenderTargetView(this->artist->getCurrentRenderTargetView(), black);
		this->artist->getDevice()->ClearDepthStencilView(this->artist->getActiveDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f, 0);
		if(this->shadows){
			this->render_objects(this->camera, this->shadow_rendering_techique, true, true, time);
		} else {
			this->render_objects(this->camera, this->normal_technique, true, true, time);
		}

		this->artist->setTextureAsRenderTarget(this->secondary_depth_texture);
		this->artist->getDevice()->ClearRenderTargetView(this->artist->getCurrentRenderTargetView(), black);
		this->artist->getDevice()->ClearDepthStencilView(this->artist->getActiveDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f, 0);
		this->render_objects(this->camera, this->depth_builder_technique, false, false, time);
		this->post_effect_manager->setupDepthImage(this->artist->getTextureManager()->createShaderResourceFromTexture(this->secondary_depth_texture));

		this->artist->setDisplayAsRenderTarget();
		this->artist->getDevice()->ClearRenderTargetView(this->artist->getCurrentRenderTargetView(), black);
		this->artist->getDevice()->ClearDepthStencilView(this->artist->getActiveDepthStencilView(), D3D10_CLEAR_DEPTH, 1.0f, 0);
		this->post_effect_manager->activate();
		if(this->draw_depth){
			this->post_effect_manager->setupImage(this->depth_texture);
		} else {
			this->post_effect_manager->setupImage(this->screen_texture);
		}
		this->post_effect_manager->prepareGPU();
		this->post_effect_manager->PostProcessEntireImage();
		if(this->dof){
			this->post_effect_manager->DOFEntireImage();
		}
		this->artist->getSwapChain()->Present(0, 0);
	}
	void BlueScreen::setup(Artist *input){
		Logger *logger = Logger::Main();
		logger->disable();
		this->only_landscape = false;
		this->another_sight = false;
		this->shadows = true;
		this->draw_depth = false;
		this->shadow_source.set(_vector3(.0f, 35.0f, .0f), _vector3(.0f, -1.0f, 0.f), _vector3(.0f, .0f, -1.0f));
		this->translucency_manager = new TranslucencyManager <>();
		Creator<>::setColor(_vector4(1.0f, 1.0f, 1.0f, 1.0f));
		this->enable_procedure_materials = true;
		this->post_effect_manager = new PostEffectManager(input);
		this->angle = .0f;
		this->artist = input;

		this->main_buffer = new VertexObject<>(input);
		this->texture_objects_buffer = new VertexObject < textured_vertex > (input);
		this->screen_objects_buffer = new VertexObject < screen_vertex > (input);

		this->main_buffer->toggleDebugMode();
		this->main_buffer->allowCPUWriteAccess();
		this->texture_objects_buffer->allowCPUWriteAccess();
		this->main_buffer->setFlexibleMode();
		this->texture_objects_buffer->setFlexibleMode();
		this->screen_objects_buffer->setFlexibleMode();

		this->landscape = new Landscape(input, this->main_buffer);
		this->landscape->setSize(25, 25);
		this->landscape->toggleSplines(false);
		this->landscape->initial_step();
		this->landscape->random_step();
		this->landscape->smooth(5);
		this->landscape->fill();
		
		this->cube = Sphere<>::create(this->main_buffer, 32);

		Creator<>::setTranslucencyManager(this->translucency_manager);

		Creator<>::setColor(_vector4(.0f, .0f, 1.0f, 0.2f));
		Creator<>::toggleTranslucency(true);
		Creator<>::setOpaque(0.3f);
		this->transparent_cube = Torus<>::create(this->main_buffer, 4);
		Creator<>::toggleTranslucency(false);
		
		/*
			Very important example for creating vertices of different types
		*/
		
		this->mirror = Mirror < textured_vertex > ::create(this->texture_objects_buffer);
		this->shape = Piramide < simple_vertex > :: create(this->main_buffer);
		
		this->main_buffer->send();
		this->texture_objects_buffer->send();


		this->main_effect = new Effect(input);
		this->main_effect->loadFromFile(L"simple.fx");

		this->normal_technique = this->main_effect->prepareTechnique("Render", Effect::NormalLayout);
		this->transparent_technique = this->main_effect->prepareTechnique("TransparentRender", Effect::NormalLayout);
		this->texturing_technique = this->main_effect->prepareTechnique("TexturingRender", Effect::TexturedLayout);
		this->building_depth_texture_technique = this->main_effect->prepareTechnique("shadow_depth_picking", Effect::NormalLayout);
		this->shadow_rendering_techique = this->main_effect->prepareTechnique("shadow_rendering", Effect::NormalLayout);
 		this->depth_builder_technique = this->main_effect->prepareTechnique("camera_depth_picker", Effect::NormalLayout);

		this->translucency_manager->setVertexObject(this->main_buffer);
		this->translucency_manager->setEffect(this->main_effect, this->transparent_technique);
		this->translucency_manager->setCameraPosition(this->camera->getPosition());

		this->light_manager = new LightManager(this->main_effect);
		this->setup_light(this->light_manager);
		D3DXMatrixPerspectiveFovLH(&this->projection_matrix, float(D3DX_PI) / 4.0f, 1.0f, .00001f, 1024.0f);
		this->screen_texture = this->artist->getTextureManager()->createRenderTexture();
		this->mirror_texture = this->artist->getTextureManager()->createRenderTexture();
		this->depth_texture = this->artist->getTextureManager()->createDepthTexture(1024);
		this->secondary_depth_texture = this->artist->getTextureManager()->createDepthTexture(1024);
		this->noise_texture = this->artist->getTextureManager()->createNoiseTexture(32, true);
		this->artist->prepareTextureRenderView(this->screen_texture);
		this->artist->prepareTextureRenderView(this->mirror_texture);
		this->artist->prepareTextureRenderView(this->depth_texture);
		this->artist->prepareTextureRenderView(this->secondary_depth_texture);
		this->procedure_material_manager = new ProcedureMaterialManager(this->artist, this->main_effect);
		this->procedure_material_manager->setNoiseTexture(this->noise_texture);
		this->marble.type = ProcedureMaterial::Strange;
		this->marble.normal_mapping = true;
		this->translucency_manager->accumulate();
		this->fog_manager = new FogManager(this->main_effect);
		Fog current;
		current.argument = .35f;
		current.color = _vector3(.0f, 1.0f, 1.0f);
		current.type = Simple;
		current.scissor_plane = _vector4(.0f, 1.0f, .0f, 2.0f);
		this->fog_manager->addFog(current);
		this->post_effect_manager->setupSceneSize(_vector2((float)this->artist->getWidth(), (float)this->artist->getHeight()));
		this->dof = false;
		/*
			TODO: fix collapse of depth buffer
		*/
		this->post_effect_manager->setupFocusData(_vector3(.999f, .9995f, 1.0f));
		this->billboard_manager = new BillboardManager(this->artist, this->main_effect);
		this->billboard_manager->allowWriting();
		this->billboard_manager->resizeInternalBuffer(1);
		Billboard billboard;
		billboard.color = _vector4(1.0f, .0f, .0f, 1.0f);
		billboard.position = _vector3(.0f, 35.0f, .0f);
		billboard.radius = 1.0f;
		billboard.type = Billboard::Sphere;
		this->billboard_manager->insertBillboard(billboard);
		this->billboard_manager->accumulate();
		this->billboard_manager->send();

		this->particle_manager = new BillboardManager(this->artist, this->main_effect);
		this->particle_manager->allowWriting();
		this->particle_manager->resizeInternalBuffer(2048);
		eplfor(i, 1024){
			Billboard billboard;
			float factor = (float)i / 4.0f;
			billboard.color = _vector4(.0f, .0f, 1.0f, 1.0f);
			billboard.position = _vector3(8.0f * cosf(factor) , 8.0f + (factor), 8.0f * sinf(factor) );
			billboard.radius = .4f;
			billboard.type = Billboard::Sphere;
			this->particle_manager->insertBillboard(billboard);
		}
		//this->particle_manager->toggleParticleMode(true);
		this->particle_manager->accumulate();
		this->particle_manager->send();
	}
	void BlueScreen::setup_light(LightManager *input){
		LightSource current;
		/*
			current.position = _vector3(.0f, 100.0f, -100.0f);
			current.color = _vector3(0.3f, 0.3f, 0.3f);
			current.intensity = 0.5f;
			current.type = Ambient;
			input->addLightSource(current);
			current.type = Dotted;
			current.color = _vector3(1.0f, 1.0f, 1.0f);
			current.position = _vector3(.0f, 100.0f, -100.0f);
			input->addLightSource(current);
			current.type = Specular;
			current.color = _vector3(1.0f, 1.0f, 1.0f);
			current.position = _vector3(.0f, 100.0f, -100.0f);
			input->addLightSource(current);
		*/
		current.lightning_model = LightSource::Specular;
		current.type = LightSource::Spot;
		current.radius = .92f;
		current.color = _vector3(1.0f, 1.0f, 1.0f);
		current.position = _vector3(.0f, 35.0f, .0f);
		current.direction = _vector3(.0f, -1.0f, .0f);
		current.attenuation = LightSource::Linear;
		current.intensity = 1.0f;
		input->addLightSource(current);
		//input->disableLightSource(0);
		/*
			current.color = _vector3(1.0f, 1.0f, 1.0f);
			current.position = _vector3(.0f, 4.0f, 2.0f);
			current.intensity = 1.0f;
			input->addLightSource(current);*/
			/*current.color = _vector3(1.0f, 1.0f, 1.0f);
			current.position = _vector3(.0f, -20.0f, -20.0f);
			current.intensity = 1.0f;
			input->addLightSource(current);
			current.color = _vector3(1.0f, 1.0f, 1.0f);
			current.position = _vector3(.0f, -20.0f, 20.0f);
			current.intensity = 1.0f;
			input->addLightSource(current);
		*/
	}
	void BlueScreen::release(){
		this->main_buffer->release();
		this->main_effect->release();
		this->landscape->release();
		if(this->translucency_manager != _n){
			delete this->translucency_manager;
			this->translucency_manager = _n;
		}
		if(this->procedure_material_manager != _n){
			delete procedure_material_manager;
			this->procedure_material_manager = _n;
		}
		if(this->billboard_manager != _n){
			delete billboard_manager;
		}
	}
	BlueScreen::BlueScreen(){
		this->mirror_texture = 0;
		this->enable_normal_mapping = false;
		this->procedure_material_manager = _n;
		this->billboard_manager = _n;
		this->cube_material = Material(_vector3(.0f, 1.0f, 1.0f));
		this->cube_material.roughness = .5f;
		this->cube_material.fresnel_reflectance = .6f;
		this->cube_material.specular_color = _vector3(1.0f, .0f, .0f);
		this->cube_material.metalness = .5f;
		this->cube_material.transparency = .0f;
		this->cube_material.smoothness = .2f;
		this->landscape_material = Material(_vector3(1.0f, 1.0f, .5f));
		this->landscape_material.roughness = .1f;
		this->landscape_material.fresnel_reflectance = .6f;
		this->landscape_material.metalness = .5f;
		this->landscape_material.transparency = .0f;
		this->landscape_material.smoothness = .5f;
		this->mirror_material = Material(_vector3(1.0f, 1.0f, 1.0f));
		this->mirror_material.roughness = .01f;
		this->mirror_material.fresnel_reflectance = .8f;
		this->mirror_material.metalness = .5f;
		this->mirror_material.transparency = .0f;
		this->mirror_material.smoothness = .3f;
		this->morph_material = Material(_vector3(.0f, 1.0f, 1.0f));
		this->morph_material.roughness = .1f;
		this->morph_material.fresnel_reflectance = .6f;
		this->morph_material.metalness = .8f;
		this->morph_material.transparency = .0f;
		this->morph_material.smoothness = .7f;
		this->main_effect = _n;
		this->landscape = _n;

		this->main_buffer = _n;
		this->texture_objects_buffer = _n;
		this->screen_objects_buffer = _n;

		this->post_effect_manager = _n;
		this->cube = 0;
		this->transparent_cube = 0;
		this->screen_texture = 0;
		this->light_manager = _n;
		this->translucency_manager = _n;
		this->fog_manager = _n;
		D3DXMatrixIdentity(&this->main_trasformation);
	}
	void BlueScreen::key_checker(Window *parent){
		if(parent->checkKey('I')) {
			this->fog_manager->disableFog(0);
		}
		if(parent->checkKey('W')){
			this->camera->move(_vector3(.0f, .0f, .1f));
		}
		if(parent->checkKey('S')){
			this->camera->move(_vector3(.0f, .0f, -.1f));
		}
		if(parent->checkKey('D')){
			this->camera->move(_vector3(.1f, .0f, .0f));
		}
		if(parent->checkKey('A')){
			this->camera->move(_vector3(-.1f, .0f, .0f));
		}
		if(parent->checkKey('Q')){
			this->camera->move(_vector3(.0f, .1f, .0f));
		}
		if(parent->checkKey('E')){
			this->camera->move(_vector3(.0f, -.1f, .0f));
		}
		if(parent->checkKey('J')){
			this->angle += (float) D3DX_PI / 200.0f;
		}
		if(parent->getCurrentPressedKey() == '7'){
			this->shadows = !this->shadows;
		}
		if(parent->getCurrentPressedKey() == '8'){
			this->draw_depth = !this->draw_depth;
		}
		/*
			TODO: fix online regenerating landscape
			if(parent->getCurrentPressedKey() == 'L'){
				this->landscape->tectonic_step();
				this->landscape->sync();
			}
		*/
		if(parent->getCurrentPressedKey() == 'M'){
			PostEffect current;
			current.type = Monochrome;
			this->post_effect_manager->addPostEffect(current);
		}
		if(parent->getCurrentPressedKey() == 'N'){
			PostEffect current;
			current.type = Saturate;
			current.alpha = 2.0f;
			this->post_effect_manager->addPostEffect(current);
		}
		if(parent->getCurrentPressedKey() == 'K'){
			this->dof = !this->dof;
		}
		if(parent->getCurrentPressedKey() == 'C'){
			this->post_effect_manager->clearPostEffects();
		}
		if(parent->getCurrentPressedKey() == 'L'){
			this->enable_procedure_materials = !this->enable_procedure_materials;
		}
		if(parent->getCurrentPressedKey() == 'O'){
			this->enable_normal_mapping = !this->enable_normal_mapping;
		}
		if(parent->getCurrentPressedKey() == 'P'){
			this->light_manager->updateLightningModel(0, LightSource::Strauss);
		}
		if(parent->getCurrentPressedKey() == 'B'){
			this->light_manager->updateLightningModel(0, LightSource::CookTorrance);
		}
		if(parent->getCurrentPressedKey() == 'V'){
			this->light_manager->updateLightningModel(0, LightSource::Specular);
		}
		if(parent->getCurrentPressedKey() == 'Z'){
			if(this->another_sight){
				*this->camera = this->saved_camera;
				this->another_sight = false;
			} else {
				this->saved_camera = *this->camera;
				*this->camera = this->shadow_source;
				this->another_sight = true;
			}
		}
	}
	BlueScreen::~BlueScreen(){
		if(this->main_effect != _n){
			delete this->main_effect;
		}
		if(this->main_buffer != _n){
			delete this->main_buffer;
		}
		if(this->light_manager != _n){
			delete this->light_manager;
		}
		if(this->landscape != _n){
			delete this->landscape;
		}
		if(this->post_effect_manager != _n){
			delete this->post_effect_manager;
		}
		if(this->procedure_material_manager != _n){
			delete this->procedure_material_manager;
		}
		if(this->fog_manager != _n){
			delete this->fog_manager;
		}
	}
	void BlueScreen::mouse_checker(_wparam wParam, int x, int y){
		static int prev_x = 0, prev_y = 0;
		int dx = 0, dy = 0;
		if(wParam && MK_LBUTTON){
			if(prev_x != 0 || prev_y != 0){
				dx = x - prev_x;
				dy = y - prev_y;
				this->camera->rotateAroundUpVector((float)dx / 500.0f);
				this->camera->rotateAroundRightVector(-(float)dy / 500.0f);
			}
			prev_x = x;
			prev_y = y;
		} else {
			prev_x = prev_y = 0;
		}
	}
}