#include "TranslucencyManager.h"
#include <map>
#include "common.h"

namespace engine{
	template <>
	_vector3 TranslucencyManager < engine::simple_vertex >::camera_position = _vector3(.0f, .0f, .0f);
}