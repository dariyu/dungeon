#ifndef __graphic_expert
#define __graphic_expert
#include "common.h"

namespace engine{
	class GraphicExpert{
	public:
		static float* createNoiseTexture(_ size, bool normal_map);
	private:
		static inline _ get_index(_ size, _ i, _ j, _ k, bool normal_map){
			if(normal_map){
				return 3 * (i + size * (j + size * k));
			} else {
				return i + size * (j + size * k);
			}
		}
		static inline _ get_near(_ size, _ index, _ modifier){
			switch(modifier){
				case 0:
					if(index == 0){
						return size - 1;
					} else {
						return index - 1;
					}
				case 1:
					return index;
				case 2:
					if(index == size - 1){
						return 0;
					} else {
						return index + 1;
					}
				default:
					return 0;
			}
		}
		static inline _vector3 get_normal(_ a, _ b, _ c){
			_vector3 result = _vector3(.0f, 1.0f, .0f);
			switch(a){
				case 0:
					result.x = 1.0f;
				case 1:
					result.x = .0f;
				case 2:
					result.x = -1.0f;
			}
			switch(c){
				case 0:
					result.z = 1.0f;
				case 1:
					result.z = .0f;
				case 2:
					result.z = -1.0f;
			}
			switch(b){
				case 0:
					result.y = 1.0f;
				case 1:
					result.y = 1.0f;
				case 2:
					result.y = -1.0f;
			}
			return result;
		}
		static _vector3 calculate_normal(float *data, _ x, _ y, _ z, _ size);
	};
}

#endif