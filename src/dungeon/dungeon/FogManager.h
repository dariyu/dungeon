#ifndef __fog_manager_
#define __fog_manager_
#include "common.h"
#include "Effect.h"

namespace engine{
	enum FogType{
		Simple
	};
	struct Fog{
		float argument;
		_vector3 color;
		_vector4 scissor_plane;
		FogType type;
	};
	class FogManager{
	public:
		const static _ max_fog_number;
		FogManager(Effect *effect);
		_ addFog(Fog input);
		void updateFogType(_ id, FogType type);
		void disableFog(_ id);
		void enableFog(_ id);
		void removeFog(_ id);
		void prepareGPU();
		void activate();
	protected:
		bool need_sync;
		bool need_update_fog_array;
		_ fog_number;
		_ counter;
		std::map < _, Fog > fogs;
		std::map < _, bool > fog_mask;
		Effect *main_effect;
	};
}
#endif