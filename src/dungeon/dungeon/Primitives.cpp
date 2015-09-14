#include "Primitives.h"
#include "common.h"
#include "Artist.h"
#include "VertexObject.h"
#include "d3d10_1.h"
#include "d3dx10.h"
#include "TranslucencyManager.h"

namespace engine{
	template <>
	bool Creator < engine::simple_vertex >::translucency = false;
	template <>
	float Creator < engine::simple_vertex >::opaque = 1.0f;
	template <>
	_vector4 Creator < engine::simple_vertex > ::current_color = _vector4(.0f, .0f, .0f, 1.0f);
	template <>
	TranslucencyManager < engine::simple_vertex > *Creator < engine::simple_vertex > ::translucency_manager = _n;
}