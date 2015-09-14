#include "VertexObject.h"
#include "VertexCompressor.h"
#include "Artist.h"
#include "common.h"
#include "d3d10_1.h"
#include "d3dx10.h"
#include <vector>
#include <stdlib.h>
#include "Logger.h"
#include "TranslucencyManager.h"

namespace engine{
	template <>
	const _ VertexObject < engine::simple_vertex > ::vertex_buffer_piece = 32 * 8;
	template <>
	const _ VertexObject < engine::simple_vertex > ::index_buffer_piece = 96 * 8;
	template <>
	const _ VertexObject < engine::textured_vertex > ::vertex_buffer_piece = 32 * 8;
	template <>
	const _ VertexObject < engine::textured_vertex > ::index_buffer_piece = 96 * 8;
	/*
		Removed method due to unused color vertex field.
		void VertexObject::fill(_ object, _vector4 color){
				if(this->already_send && this->updating_buffer != _n){
					for(_ i = 0; i < this->updating_vertices_count; i++){
						this->updating_buffer[i].color = color;
					}
					return;
				}
				model_info current = this->models[object];
				_ iv = current.index_shift;
				_ end_vertex = current.index_shift + current.vertices_number;
				while(iv < end_vertex){
					this->vertex_buffer_data[iv].color = color;
					iv++;
				}
			}
	*/
}