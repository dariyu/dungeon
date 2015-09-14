#ifndef __translucency_manager
#define __translucency_manager

#include "Effect.h"
#include "common.h"
#include <map>
#include "d3dx10.h"
#include "VertexObject.h"

namespace engine{
	template < class T > class VertexObject;
	/*
		Some important information about this manager:
			- it serves translucency geometry.
			- it can apply one effect to all drawing polygons
			- it use current index buffer for drawing faces
	*/
	template < class T = engine::simple_vertex >
	class TranslucencyManager{
	public:
		struct Triangle{
			_vector3 center;
			_ start_index;
			_ parent;
			inline Triangle() : center(_vector3(.0f, .0f, .0f)), start_index(0), parent(0) {}
			inline Triangle(_vector3 &center, _ start_index, _ parent) : center(center), start_index(start_index), parent(parent) {}
			inline void transform(_vector3 &input, _matrix &tranformation){
				_vector4 result;
				D3DXVec3Transform(&result, &this->center, &tranformation);
				input.x = result.x, input.y = result.y, input.z = result.z;
			}
		};
		struct TransparentObject{
			bool transformed;
			_matrix transformation;
			_ start_index, index_shift, indices_count;
			float opaque;
			TransparentObject() {
				this->opaque = 1.0f;
				this->transformed = false;
				D3DXMatrixIdentity(&this->transformation);
				this->start_index = this->index_shift = this->indices_count = 0;
			}
		};
		TranslucencyManager(){
			this->main_effect = _n;
			this->sorting = _n;
			this->cache = _n;
			this->sorting_buffer = _n;
			this->vertex_buffer = _n;
			this->main_technique = 0;
		}
		~TranslucencyManager(){
			if(this->sorting != _n)
				delete this->sorting;
			if(this->cache != _n)
				delete this->cache;
			if(this->sorting_buffer != _n)
				delete this->sorting_buffer;
		}
		/*
			Simple setters.
		*/
		void registerTransparentObject(_ object, _ start_index, _ indices_count, _ index_shift, float opaque){
			TransparentObject current;
			current.start_index = start_index;
			current.index_shift = index_shift;
			current.indices_count = indices_count;
			current.opaque = opaque;
			this->objects[object] = current;
		}
		void insertTransparentTriangle(_ object, _vector3 &center, _ start_index){
			this->triangles[this->triangles.size()] = Triangle(center, start_index, object);
			this->children[object][this->children[object].size()] = this->triangles.size() - 1;
		}
		void setCameraPosition(_vector3 &position){
			this->camera_position = position;
		}
		void setEffect(Effect *effect, _ technique){
			this->main_effect = effect;
			this->main_technique = technique;
		}
		void setObjectTransformation(_ object, _matrix &trasformation){
			if(this->objects[object].transformation != trasformation){
				this->objects[object].transformation = trasformation;
				this->objects[object].transformed = true;
			}
		}
		void setVertexObject(VertexObject < T > *vertex_buffer){
			this->vertex_buffer = vertex_buffer;
		}
		/*
			Transfer all data to fast sorting buffers.
		*/
		void accumulate(){
			this->sorting = new Triangle[this->triangles.size()];
			this->cache = new Triangle*[this->triangles.size()];
			this->sorting_buffer = new Triangle*[this->triangles.size()];
			eplfor(i, this->triangles.size()){
				this->cache[i] = &this->sorting[i];
			}
			mapfor(i, _, TransparentObject, this->objects){
				mapfor(j, _, _, this->children[i->first]){
					this->sorting[j->second] = this->triangles[j->second];
				}
			}
		}
		/*
			Main method for sorting. (it can be started simultaneously with frame rendering in separate thread.
			When all work is done we can draw all transparent faces in back to front order.
		*/
		void sort(){
			mapfor(i, _, TransparentObject, this->objects){
				if(i->second.transformed){
					mapfor(j, _, _, this->children[i->first]){
						this->triangles[j->second].transform(this->sorting[j->second].center, i->second.transformation);
					}
					i->second.transformed = false;
				}
			}
			common_merge_sort((void*)this->cache, (int)this->triangles.size(), sizeof(Triangle*), TranslucencyManager::vector_compare, (void*)sorting_buffer);
		}
		/*
			Must be called after finishing sort method execution.
		*/
		void draw(){
			float current_opaque = 1.0f;
			if(this->vertex_buffer != _n){
				eplfor(i, this->triangles.size()){
					float t = this->objects[this->sorting[i].parent].opaque;
					if(t != current_opaque){
						this->main_effect->getVariable("opaque")->AsScalar()->SetFloat(t);
						current_opaque = t;
					}
					if(this->main_effect != _n)
						this->main_effect->applyTechnique(this->main_technique);
					this->vertex_buffer->drawTriangle(this->sorting[i].parent, this->sorting[i].start_index);
				}
			}
			if(current_opaque != 1.0f){
				this->main_effect->getVariable("opaque")->AsScalar()->SetFloat(1.0f);
			}
		}
		bool isTransparent(_ object_id){
			return this->objects.count(object_id) != 0;
		}
	protected:
		static int vector_compare(const void *first, const void *second){
			_vector3 *alpha = &(*(Triangle**)first)->center;
			_vector3 *beta = &(*(Triangle**)second)->center;
			_vector3 current = *alpha - TranslucencyManager::camera_position;
			double a = D3DXVec3LengthSq(&current);
			current = *beta - TranslucencyManager::camera_position;
			double b = D3DXVec3LengthSq(&current);
			if(a > b)
				return 1;
			if(a < b)
				return -1;
			return 0;
		}
		std::map < _, std::map < _, _ > > children;
		std::map < _, Triangle > triangles;
		std::map < _, TransparentObject > objects;
		/*
			Fast array for storing triangle centers.
		*/
		Triangle *sorting;
		/*
			Fast array for storing order of drawing triangles: only pointers will be swapped while sorting is performed.
		*/
		Triangle **cache;
		/*
			This buffer is required for common_merge_sort function
		*/
		Triangle **sorting_buffer;
		static _vector3 camera_position;
		Effect *main_effect;
		VertexObject < T > *vertex_buffer;
		_ main_technique;
	};
}
#endif