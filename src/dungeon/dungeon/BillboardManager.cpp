#include "BillboardManager.h"
#include "common.h"
#include "Effect.h"
#include "Vertices.h"
#include <stdlib.h>

namespace engine{
	_vector3 BillboardManager::camera_position = _vector3(.0f, .0f, .0f);
	BillboardManager::BillboardManager(Artist *artist, Effect *effect){
		this->main_effect = effect;
		this->artist = artist;
		this->vertices = _n;
		this->vertices_count = 0;
		this->vertices_limit = 0;
		this->vertex_buffer = _n;
		this->allow_writing = false;
		this->particle_mode = false;
		this->cache = _n;
		this->sorting = _n;
		this->sorting_buffer = _n;
		this->main_technique = this->main_effect->prepareTechnique("billboard_rendering", Effect::BillboardLayout);
	}
	void BillboardManager::toggleParticleMode(bool input){
		this->particle_mode = input;
		this->particle_technique = this->main_effect->prepareTechnique("particle_rendering", Effect::BillboardLayout);
	}
	int BillboardManager::point_compare(const void *first, const void *second){
		_vector3 *alpha = &(*(Point**)first)->center;
		_vector3 *beta = &(*(Point**)second)->center;
		_vector3 current = *alpha - BillboardManager::camera_position;
		double a = D3DXVec3LengthSq(&current);
		current = *beta - BillboardManager::camera_position;
		double b = D3DXVec3LengthSq(&current);
		if(a > b)
			return 1;
		if(a < b)
			return -1;
		return 0;
	}
	void BillboardManager::resizeInternalBuffer(_ size){
		this->vertices_limit += size;
		this->vertices = (billboard_vertex*)realloc((void*)this->vertices, this->vertices_limit * sizeof(billboard_vertex));
	}
	void BillboardManager::sort(){
		common_merge_sort((void*)this->cache, (int)this->points.size(), sizeof(Point*), BillboardManager::point_compare, (void*)sorting_buffer);
	}
	void BillboardManager::setupCamera(_vector3 position){
		BillboardManager::camera_position = position;
	}
	void BillboardManager::insertBillboard(Billboard &input){
		billboard_vertex current;
		current.position = input.position;
		current.color = input.color;
		current.center = input.position;
		current.type = input.type;
		current.radius = input.radius;
		this->points.push_back(Point(current.position, this->vertices_count));
		this->vertices[this->vertices_count++] = current;
	}
	void BillboardManager::allowWriting(){
		this->allow_writing = true;
	}
	void BillboardManager::accumulate(){
		this->sorting = new Point[this->points.size()];
		this->cache = new Point*[this->points.size()];
		this->sorting_buffer = new Point*[this->points.size()];
		eplfor(i, this->points.size()){
			this->cache[i] = &this->sorting[i];
			this->sorting[i].parent = this->points[i].parent;
			this->sorting[i].center = this->points[i].center;
		}
	}
	void BillboardManager::send(){
		HRESULT hr = S_OK;
		D3D10_BUFFER_DESC buffer_description;
		buffer_description.ByteWidth = (UINT)(sizeof(billboard_vertex) * this->vertices_count);
		buffer_description.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		if(this->allow_writing) {
			buffer_description.Usage = D3D10_USAGE_DYNAMIC;
			buffer_description.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		} else {
			buffer_description.Usage = D3D10_USAGE_DEFAULT;
			buffer_description.CPUAccessFlags = 0;
		}
		buffer_description.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA data;
		data.pSysMem = this->vertices;
		if(FAILED(this->artist->getDevice()->CreateBuffer(&buffer_description, &data, &this->vertex_buffer)))
			return;
		if(!this->allow_writing){
			free(this->vertices);
			this->vertices = _n;
		}
	}
	BillboardManager::~BillboardManager(){
		if(this->vertices != _n){
			free(this->vertices);
		}
		if(this->sorting != _n){
			delete this->sorting;
		}
		if(this->cache != _n){
			delete this->cache;
		}
		if(this->sorting_buffer != _n){
			delete this->sorting_buffer;
		}
	}
	void BillboardManager::activate(){
		UINT stride = sizeof(billboard_vertex);
		UINT offset = 0;
		this->artist->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		this->artist->getDevice()->IASetVertexBuffers(0, 1, &this->vertex_buffer, &stride, &offset);
		this->main_effect->activateTechniqueLayout(this->main_technique);
	}
	void BillboardManager::draw(){
		if(this->vertices != _n){
			if(this->particle_mode){
				eplfor(i, this->points.size()){
					this->main_effect->applyTechnique(this->particle_technique);
					this->artist->getDevice()->Draw(1, (UINT)this->sorting[i].parent);
				}
			} else {
				eplfor(i, this->points.size()){
					this->main_effect->applyTechnique(this->main_technique);
					this->artist->getDevice()->Draw(1, (UINT)this->sorting[i].parent);
				}
			}
		}
	}
}