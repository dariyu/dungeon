#ifndef __blue_screen
#define __blue_screen

#include "Scene.h"
#include "Effect.h"
#include "Artist.h"
#include "LightManager.h"
#include "PostEffectManager.h"
#include "ProcedureMaterialManager.h"
#include "TranslucencyManager.h"
#include "FogManager.h"
#include "BillboardManager.h"

namespace engine{
	class Scene;
	class Effect;
	class BlueScreen : public Scene{
	public:
		BlueScreen();
		void prepare_shadows(Camera *current);
		void render();
		void setup(Artist *input);
		void key_checker(Window *parent);
		void release();
		virtual ~BlueScreen();
		void mouse_checker(_wparam wParam, int x, int y);
	private:
		bool dof, draw_depth;
		bool only_landscape;
		bool another_sight, shadows;
		Camera shadow_source, saved_camera;
		_ depth_texture, secondary_depth_texture;
		_ building_depth_texture_technique, shadow_rendering_techique, texturing_technique, depth_builder_technique;
		Camera portal_camera;
		void render_objects(Camera *camera, _ main_technigue, bool draw_transparent, bool draw_mirrors, float time);
		void setup_light(LightManager *input);
		Material cube_material, landscape_material, mirror_material, morph_material;
		LightManager *light_manager;
		PostEffectManager *post_effect_manager;
		ProcedureMaterialManager *procedure_material_manager;
		FogManager *fog_manager;
		BillboardManager *billboard_manager;
		BillboardManager *particle_manager;
		ProcedureMaterial marble;
		TranslucencyManager < simple_vertex > *translucency_manager;
		float angle;
		bool enable_normal_mapping;
		_ cube, mirror, shape;
		_ transparent_cube;
		_ screen_texture, noise_texture, mirror_texture;
		_ normal_technique, transparent_technique, post_effect_technique;
		_matrix main_trasformation;
		Effect *main_effect;
		VertexObject < simple_vertex > *main_buffer;
		VertexObject < textured_vertex > *texture_objects_buffer;
		VertexObject < screen_vertex > *screen_objects_buffer;
		Landscape *landscape;
		bool enable_procedure_materials;
	};
}

#endif