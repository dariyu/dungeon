#include "Landscape.h"
#include "common.h"
#include "d3d10_1.h"
#include "d3dx10.h"
#include "Artist.h"
#include "Weaver.h"
#include <stdlib.h>
#include <cmath>

namespace engine{
	Landscape::Landscape(Artist *parent, VertexObject < engine::simple_vertex > *input){
		this->parent = parent;
		this->erosive_step_number = 0;
		this->tectonic_step_number = 0;
		this->width = this->height = 0;
		if(input == _n){
			this->object = new VertexObject < engine::simple_vertex > (parent);
			this->shared_vertex_object = false;
		} else {
			this->object = input;
			this->shared_vertex_object = true;
		}
		this->data = _n;
		this->tension = _n;
		this->used = _n;
		this->id = 0;
		this->enable_splines = false;
	}
	void Landscape::toggleSplines(bool input){
		this->enable_splines = input;
	}
	Landscape::~Landscape(){
		if(!this->shared_vertex_object)
			delete this->object;
	}
	void Landscape::setSize(_ width, _ height){
		this->width = width;
		this->height = height;
	}
	void Landscape::release(){
		if(!this->shared_vertex_object){
			this->object->release();
		}
		if(this->data != _n){
			delete this->data;
		}
		if(this->tension != _n){
			delete this->tension;
		}
		if(this->used != _n){
			delete this->used;
		}
	}
	void Landscape::draw(){
		this->object->draw(this->id);
	}
	_vector3 Landscape::getVertex(_ x, _ y){
		_vector3 result = _vector3(.0f, .0f, .0f);
		result.y = this->data[this->getIndex(x, y)];
		result.x = (float)x / (float)this->width - .5f;
		result.z = (float)y / (float)this->height - .5f;
		return result;
	}
	_ Landscape::getIndex(_ x, _ y){
		return this->width * x + y;
	}
	void Landscape::initial_step(){
		this->data = new float[this->height * this->width];
		this->tension = new float[this->height * this->width];
		this->used = new bool[this->height * this->width];
		ZeroMemory(this->data, sizeof(float) * this->height * this->width);
		ZeroMemory(this->tension, sizeof(float) * this->height * this->width);
		ZeroMemory(this->used, sizeof(bool) * this->height * this->width);
		/*_ size = 1;
		if(this->height != 1){
			size *= this->height + (this->height - 3) / 2;
		} else {
			size *= this->height;
		}
		if(this->width != 1){
			size *= this->width + (this->width - 3) / 2;
		} else {
			size *= this->width;
		}
		size /= 3;
		size *= 8;
		this->object->resizeInternalBuffers(this->height * this->width, size);*/
	}
	simple_vertex* Landscape::build_data(){
		simple_vertex *result = this->object->getUnsafeAccess(this->id);
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				result[this->getIndex(x, y)] = this->getVertex(x, y);
			}
		}
		return result;
	}
	void Landscape::fill(){
		/*
			//A beautiful example of using Weaver class
			this->object->resizeInternalBuffers(this->height * this->width, 0);
			Weaver<> *local_weaver = new Weaver<>(this->object);
			if(this->enable_splines)
				local_weaver->begin(Weaver<>::Patches);
			else
				local_weaver->begin(Weaver<>::Quads);
			for(_ x = 0; x < this->width; x++){
				for(_ y = 0; y < this->height; y++){
					local_weaver->insert_vertex(this->getVertex(x, y));
				}
			}
			for(_ x = 1; x < this->width; x += 2){
				for(_ y = 1; y < this->height; y += 2){
					local_weaver->insert_primitive_index(this->getIndex(x, y));
					local_weaver->insert_primitive_index(this->getIndex(x - 1, y));
					local_weaver->insert_primitive_index(this->getIndex(x - 1, y + 1));
	
					local_weaver->insert_primitive_index(this->getIndex(x, y + 1));
					local_weaver->insert_primitive_index(this->getIndex(x + 1, y + 1));
					local_weaver->insert_primitive_index(this->getIndex(x + 1, y));
	
					local_weaver->insert_primitive_index(this->getIndex(x + 1, y - 1));
					local_weaver->insert_primitive_index(this->getIndex(x, y - 1));
					local_weaver->insert_primitive_index(this->getIndex(x - 1, y - 1));
				}
			}
			local_weaver->end();
			delete local_weaver;
		*/
		_ size = 1;
		if(this->height != 1){
			size *= this->height + (this->height - 3) / 2;
		} else {
			size *= this->height;
		}
		if(this->width != 1){
			size *= this->width + (this->width - 3) / 2;
		} else {
			size *= this->width;
		}
		size /= 3;
		size *= 8;
		this->object->resizeInternalBuffers(this->height * this->width, size);
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				simple_vertex current = this->getVertex(x, y);
				current.position.x *= (float)this->width;
				current.position.z *= (float)this->height;
				this->object->insert_vertex(current);
			}
		}
		this->object->swapVertexOrder();
		for(_ x = 1; x < this->width; x += 2){
			for(_ y = 1; y < this->height; y += 2){
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x, y + 1), this->getIndex(x - 1, y + 1));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x - 1, y + 1), this->getIndex(x - 1, y));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x - 1, y), this->getIndex(x - 1, y - 1));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x - 1, y - 1), this->getIndex(x, y - 1));

				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x, y - 1), this->getIndex(x + 1, y - 1));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x + 1, y - 1), this->getIndex(x + 1, y));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x + 1, y), this->getIndex(x + 1, y + 1));
				this->object->insert_triangle(this->getIndex(x, y), this->getIndex(x + 1, y + 1), this->getIndex(x, y + 1));
			}
		}
		this->object->swapVertexOrder();
		this->id = this->object->freeze();
		this->object->calculate_normals(this->id);
		if(!this->shared_vertex_object){
			this->object->send();
		}
	}
	void Landscape::smooth(_ detalization){
		Smoother *smoother = new Smoother();
		smoother->setSmoothFactor(detalization);
		float *result = smoother->smoothHeightMap(this->data, this->width, this->height);
		delete this->data;
		this->data = result;
		this->width = smoother->getWidth();
		this->height = smoother->getHeight();
		delete smoother;
	}
	void Landscape::place_vulcan(_ vx, _ vy, _ force){
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				float delta = .0f;
				float dist = pow(fabs((float)x - (float)vx), 2) + pow(fabs((float)y - (float)vy), 2);
				delta = 200.0f - 0.06f * dist;
				if(delta > .0f)
					this->data[this->getIndex(x,y)] += delta;
			}
		}
	}
	void Landscape::tension_simulator(_ tx, _ ty, _ dx, _ dy, _ force){
	}
	void Landscape::linear_step(){
		if(this->tension == _n || this->data == _n)
			return;
		ZeroMemory(this->tension, sizeof(float) * this->width * this->height);
		ZeroMemory(this->data, sizeof(float) * this->width * this->height);
		srand(GetTickCount());
		float max_height = 0.0f;
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] = (float)pow((double)y, 12.0);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				max_height = max(this->data[this->getIndex(x, y)], max_height);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] /= max_height * 0.5f;
			}
		}
		/*for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				if(this->data[this->getIndex(x, y)] > 1.0f){
					float a = 1.0f;
				}
			}
		}*/
	}
	void Landscape::tectonic_step(){
		if(this->tension == _n || this->data == _n)
			return;
		ZeroMemory(this->tension, sizeof(float) * this->width * this->height);
		ZeroMemory(this->data, sizeof(float) * this->width * this->height);
		srand(GetTickCount());
		_ vulcan_number = 10 + rand() % 100;
		float max_height = 0.0f;
		for(_ i = 0; i < vulcan_number; i++){
			ZeroMemory(this->used, sizeof(bool) * this->width * this->height);
			this->place_vulcan(rand() % this->width, rand() % this->height, 5 + rand() % 20);
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				max_height = max(this->data[this->getIndex(x, y)], max_height);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] /= max_height * 0.1f;
			}
		}
	}
	void Landscape::random_step(){
		if(this->tension == _n || this->data == _n)
			return;
		ZeroMemory(this->tension, sizeof(float) * this->width * this->height);
		ZeroMemory(this->data, sizeof(float) * this->width * this->height);
		srand(GetTickCount());
		float max_height = 0.0f;
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] = (float)(rand() % 100);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				max_height = max(this->data[this->getIndex(x, y)], max_height);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] /= max_height * 0.1f;
			}
		}
	}
	void Landscape::plasma_generator(_ xa, _ ya, _ xb, _ yb){
		srand(GetTickCount());
		/*_ vulcan_number = rand() % 100;
		float max_height = 0.0f;
		for(_ i = 0; i < vulcan_number; i++){
			this->place_vulcan(rand() % this->width, rand() % this->height, 5);
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				max_height = max(this->data[this->getIndex(x, y)], max_height);
			}
		}
		for(_ x = 0; x < this->width; x++){
			for(_ y = 0; y < this->height; y++){
				this->data[this->getIndex(x, y)] /= max_height * 0.1f;
			}
		}*/
	}
	void Landscape::sync(){
		simple_vertex *result = this->build_data();
		//this->object->fill(this->id, _vector4(0.0f, 1.0f, 0.0f, 1.0f));
		this->object->calculate_normals(this->id);
		this->object->vacuum();
	}
	void Landscape::erosive_step(){
	}
}