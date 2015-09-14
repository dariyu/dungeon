#ifndef __procedure_material_manager
#define __procedure_material_manager

#include "Effect.h"
#include "Artist.h"

namespace engine{
	struct ProcedureMaterial{
		enum MaterialType{
			Marble = 0,
			Wood = 1,
			Strange = 2,
			Plasma = 3
		};
		ProcedureMaterial() : type(Marble), normal_mapping(false) {
		}
		MaterialType type;
		bool normal_mapping;
	};
	class ProcedureMaterialManager{
	public:
		ProcedureMaterialManager(Artist *artist, Effect *effect);
		void setMaterial(ProcedureMaterial alpha);
		void toggleProcessingProcedureMaterials(bool alpha);
		void PrepareGPU();
		void setNoiseTexture(_ id);
	protected:
		Artist *artist;
		bool need_sync;
		bool enabled;
		bool noise_texture_loaded;
		ProcedureMaterial current;
		Effect *main_effect;
		_ noise_texture;
	};
}
#endif