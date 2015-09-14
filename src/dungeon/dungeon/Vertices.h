#ifndef __vertices
#define __vertices
#include "common.h"

namespace engine{
	struct billboard_vertex{
		_vector3 position;
		_vector3 center;
		_vector4 color;
		float radius;
		_uint8 type;
	};
	struct screen_vertex{
		_vector4 position;
		_vector2 texture_coordinate;
	};
	struct textured_vertex{
		_vector3 position;
		_vector3 normal;
		_vector2 texture_coordinate;
		inline textured_vertex(){
			this->position = _vector3(.0f, .0f, .0f);
			this->normal = _vector3(.0f, .0f, .0f);
			this->texture_coordinate = _vector2(.0f, .0f);
		}
		inline textured_vertex(_vector3 pos){
			this->position = pos;
			this->normal = _vector3(.0f, .0f, .0f);
			this->texture_coordinate = _vector2(.0f, .0f);
		}
		inline textured_vertex(_vector3 pos, _vector2 texture_coordinate){
			this->position = pos;
			this->texture_coordinate = texture_coordinate;
			this->normal = _vector3(.0f, .0f, .0f);
		}
		static bool less_than(textured_vertex *alpha, textured_vertex *beta){
			if(alpha->position != beta->position)
				return alpha->position < beta->position;
			if(alpha->normal != beta->normal)
				return alpha->normal < beta->position;
			return false;
		}
		static bool greater_than(textured_vertex *alpha, textured_vertex *beta){
			return !textured_vertex::less_than(alpha, beta);
		}
		static bool equal(textured_vertex *alpha, textured_vertex * beta){
			if(alpha->position != beta->position)
				return false;
			if(alpha->normal != beta->position)
				return false;
			return true;
		}
	};
	struct simple_vertex{ 
		_vector3 position;
		_vector3 normal;
		inline simple_vertex(_vector3 pos){
			this->position = pos;
			this->normal = _vector3(.0f, .0f, .0f);
		}
		inline simple_vertex() {
			this->position = _vector3(.0f, .0f, .0f);
			this->normal = _vector3(.0f, .0f, .0f);
		}
		static bool less_than(simple_vertex *alpha, simple_vertex *beta){
			if(alpha->position != beta->position)
				return alpha->position < beta->position;
			if(alpha->normal != beta->normal)
				return alpha->normal < beta->position;
			return false;
		}
		static bool greater_than(simple_vertex *alpha, simple_vertex *beta){
			return !simple_vertex::less_than(alpha, beta);
		}
		static bool equal(simple_vertex *alpha, simple_vertex * beta){
			if(alpha->position != beta->position)
				return false;
			if(alpha->normal != beta->normal)
				return false;
			return true;
		}
	};
	struct color_vertex{ 
		_vector3 position;
		_vector4 color;
		_vector3 normal;
		inline color_vertex(_vector3 pos, _vector4 color){
			this->position = pos;
			this->color = color;
			this->normal = _vector3(.0f, .0f, .0f);
		}
		inline color_vertex(_vector3 pos){
			this->position = pos;
			this->color = _vector4(.0f, .0f, .0f, 1.0f);
			this->normal = _vector3(.0f, .0f, .0f);
		}
		inline color_vertex() {
			this->position = _vector3(.0f, .0f, .0f);
			this->color = _vector4(.0f, .0f, .0f, 1.0f);
			this->normal = _vector3(.0f, .0f, .0f);
		}
		static bool less_than(color_vertex *alpha, color_vertex *beta){
			if(alpha->position != beta->position)
				return alpha->position < beta->position;
			if(alpha->normal != beta->normal)
				return alpha->normal < beta->normal;
			if(alpha->color != beta->color)
				return alpha->color < beta->color;
			return false;
		}
		static bool greater_than(color_vertex *alpha, color_vertex *beta){
			return !color_vertex::less_than(alpha, beta);
		}
		static bool equal(color_vertex *alpha, color_vertex * beta){
			if(alpha->position != beta->position)
				return false;
			if(alpha->normal != beta->normal)
				return false;
			if(alpha->color != beta->color)
				return false;
			return true;
		}
	};
}
#endif