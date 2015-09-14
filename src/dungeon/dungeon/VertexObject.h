#ifndef __vertex_object
#define __vertex_object

#include "Artist.h"
#include "VertexCompressor.h"
#include "Vertices.h"
#include "TranslucencyManager.h"
#include "Logger.h"
#include <assert.h>

namespace engine{

	template < class T = engine::simple_vertex > class VertexCompressor;
	template < class T = engine::simple_vertex > class TranslucencyManager;

	struct _line{
		_vector3 from;
		_vector3 to;
	};

	template < class T = engine::simple_vertex>
	class VertexObject{
	public:
		void swapVertexOrder(){
			this->normal_vertex_order = !this->normal_vertex_order;
		}
		VertexObject(Artist *parent){
			this->parent = parent;
			this->vertex_buffer_size = 0;
			this->index_buffer_size = 0;
			this->index_buffer_data = _n;
			this->debug_buffer_data = _n;
			this->vertex_buffer_data = _n;
			this->current_vertex = 0;
			this->current_index = 0;
			this->index_buffer = _n;
			this->vertex_buffer = _n;
			this->debug_buffer = _n;
			this->flexible = false;
			this->index_buffer_size_limit = this->vertex_buffer_size_limit = 0;
			this->updating_vertex_start = this->updating_vertices_count = 0;
			this->updating_buffer = _n;
			this->allow_writing = false;
			this->already_send = false;
			this->debug_mode = false;
			this->compressor = new VertexCompressor < T > ();
			this->compress_mode = false;
			this->normal_vertex_order = true;
			this->correction_mode = false;
		}
		~VertexObject(){
			delete this->compressor;
		}
		void toggleNormalsCorrection(bool correction_mode){
			this->correction_mode = correction_mode;
		}
		HRESULT resizeInternalBuffers(_ vertex_buffer_size, _ index_buffer_size){
			if(this->flexible) {
				this->vertex_buffer_size_limit += vertex_buffer_size;
				this->index_buffer_size_limit += index_buffer_size;
				if(this->vertex_buffer_size_limit != 0){
					this->vertex_buffer_data = (T*) realloc(this->vertex_buffer_data, sizeof(T) * this->vertex_buffer_size_limit);
				}
				if(this->index_buffer_size_limit != 0){
					this->index_buffer_data = (_d*) realloc(this->index_buffer_data, sizeof(_d) * this->index_buffer_size_limit);
				}
			} else {
				this->vertex_buffer_size = vertex_buffer_size;
				this->index_buffer_size = index_buffer_size;
				if(this->vertex_buffer_size != 0){
					this->vertex_buffer_data = new T[this->vertex_buffer_size];
				}
				if(this->index_buffer_size != 0){
					this->index_buffer_data = new _d[this->index_buffer_size];
				}
			}
			return S_OK;
		}
		void setFlexibleMode(){
			this->flexible = true;
			this->index_buffer_size_limit = 0;
			this->vertex_buffer_size_limit = 0;
		}
		_ insert_vertex(T input){
			Logger *logger = Logger::Main();
			logger->log(L"Vertex:\n");
			logger->log(&input);
			logger->log(L"\n");
			if(!flexible && this->current_vertex == this->vertex_buffer_size){
				return 0;
			}
			if(this->compress_mode){
				if(this->compressor->exists(&input)){
					return this->compressor->getId(&input);
				}
			}
			if(flexible){
				if(this->current_vertex == this->vertex_buffer_size_limit) {
					this->vertex_buffer_size_limit += VertexObject::vertex_buffer_piece;
					this->vertex_buffer_data = (T*) realloc(this->vertex_buffer_data, sizeof(T) * this->vertex_buffer_size_limit);
				}
			}
			this->vertex_buffer_data[this->current_vertex++] = input;
			_ id = this->current_vertex - this->last_model.index_shift - this->last_model.vertices_number - 1;
			if(this->compress_mode){
				this->compressor->insert_vertex(this, id);
			}
			return id;
		}
		HRESULT insert_triangle(_ alpha, _ beta, _ gamma){
			Logger *logger = Logger::Main();
			if(!flexible && this->current_vertex == this->index_buffer_size){
				return S_FALSE;
			}
			if(flexible) {
				if(this->current_index == this->index_buffer_size_limit) {
					this->index_buffer_size_limit += VertexObject::index_buffer_piece;
					this->index_buffer_data = (_d*) realloc(this->index_buffer_data, sizeof(_d) * this->index_buffer_size_limit);
				}
			}
			if(this->normal_vertex_order){
				this->index_buffer_data[this->current_index++] = (_d)alpha;
				this->index_buffer_data[this->current_index++] = (_d)beta;
				this->index_buffer_data[this->current_index++] = (_d)gamma;
			} else {
				this->index_buffer_data[this->current_index++] = (_d)gamma;
				this->index_buffer_data[this->current_index++] = (_d)beta;
				this->index_buffer_data[this->current_index++] = (_d)alpha;
			}
			logger->log(L"Triangle: ");
			logger->begin(Logger::List);
			logger->log(alpha);
			logger->log(beta);
			logger->log(gamma);
			logger->end();
			logger->log(L"\n");
			return S_OK;
		}
		/*
			Caution: CCW vertex order
			This function doesn't create new vertex, so we can apply it to any object, stored in vertex object buffer.
		*/
		void calculate_normals(_ object){
			model_info current = this->models[object];
			_ iv = current.index_shift;
			_ end_vertex = current.index_shift + current.vertices_number;
			_ ii = 0;
			if(this->already_send && this->updating_buffer != _n){
				while(ii < current.indices_number){
					if(!(this->exists(ii, object) && this->exists(ii + 1, object) && this->exists(ii + 2, object)))
						continue;
					_vector3 alpha = this->getVertex(ii + 1, object).position - this->getVertex(ii, object).position;
					_vector3 beta = this->getVertex(ii + 2, object).position - this->getVertex(ii, object).position;
					_vector3 normal = _vector3(.0f, .0f, .0f);
					D3DXVec3Cross(&normal, &alpha, &beta);
					D3DXVec3Normalize(&normal, &normal);
					if(this->correction_mode){
						if(normal.y < 0){
							normal = -normal;
						}
					}
					this->getVertex(ii, object).normal += normal;
					this->getVertex(ii + 1, object).normal += normal;
					this->getVertex(ii + 2, object).normal += normal;
					ii += 3;
				}
				iv = 0;
				while(iv < this->updating_vertices_count){
					D3DXVec3Normalize(&this->updating_buffer[iv].normal, &this->updating_buffer[iv].normal);
					iv++;
				}
			} else {
				while(ii < current.indices_number){
					_vector3 alpha = this->getVertex(ii + 1, object).position - this->getVertex(ii, object).position;
					_vector3 beta = this->getVertex(ii + 2, object).position - this->getVertex(ii, object).position;
					_vector3 normal = _vector3(.0f, .0f, .0f);
					D3DXVec3Cross(&normal, &alpha, &beta);
					D3DXVec3Normalize(&normal, &normal);
					if(this->correction_mode){
						if(normal.y < 0){
							normal = -normal;
						}
					}
					this->getVertex(ii, object).normal += normal;
					this->getVertex(ii + 1, object).normal += normal;
					this->getVertex(ii + 2, object).normal += normal;
					ii += 3;
				}
				iv = current.index_shift;
				while(iv < end_vertex){
					_line current;
					D3DXVec3Normalize(&this->vertex_buffer_data[iv].normal, &this->vertex_buffer_data[iv].normal);
					current.from = this->vertex_buffer_data[iv].normal;
					current.to = this->vertex_buffer_data[iv].position;
					normals.push_back(current);
					iv++;
				}
			}
		}
		void advanced_calculate_normals_for_last_model(){
			_ last = this->models.size() - 1;
			model_info current = this->models[last];
			_ iv = current.index_shift;
			_ ii = 0;
			std::vector < T > new_vertex_buffer;
			_ new_vertex_counter = 0;
			while(ii < current.indices_number){
				_vector3 alpha = this->getVertex(ii + 1, last).position - this->getVertex(ii, last).position;
				_vector3 beta = this->getVertex(ii + 2, last).position - this->getVertex(ii, last).position;
				_vector3 normal = _vector3(.0f, .0f, .0f);
				D3DXVec3Cross(&normal, &alpha, &beta);
				D3DXVec3Normalize(&normal, &normal);
				for(_ j = 0; j < 3; j++){
					if(this->checkForDuplication(this->getVertex(ii, last).normal, normal)){
						new_vertex_buffer.push_back(this->getVertex(ii, last));
						new_vertex_buffer[new_vertex_counter].normal = normal;
						this->index_buffer_data[ii + current.start_index] = (_d)(current.vertices_number + new_vertex_counter);
						new_vertex_counter++;
					} else {
						this->getVertex(ii, last).normal += normal;
					}
					ii++;
				}
			}

			iv = 0;
			this->resizeInternalBuffers(new_vertex_counter, 0);
			while(iv < new_vertex_counter){
				this->vertex_buffer_data[current.index_shift + current.vertices_number + iv] = new_vertex_buffer[iv];
				iv++;
			}
			this->models[last].vertices_number += new_vertex_counter;
			iv = current.index_shift;
			this->vertex_buffer_size += new_vertex_counter;
			this->current_vertex += new_vertex_counter;
			while(iv < this->vertex_buffer_size){
				D3DXVec3Normalize(&this->vertex_buffer_data[iv].normal, &this->vertex_buffer_data[iv].normal);
				iv++;
				/*
					_line current;
					current.from = this->vertex_buffer_data[iv].normal;
					current.to = this->vertex_buffer_data[iv].position;
					normals.push_back(current);
				*/
			}
			this->last_model = this->models[last];
		}
		void draw(_ object){
			this->parent->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			model_info model = this->models[object];
			this->parent->getDevice()->DrawIndexed((UINT)model.indices_number, (UINT)model.start_index, (UINT)model.index_shift);
		}
		void drawTriangle(_ object, _ start_index){
			this->parent->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			model_info model = this->models[object];
			this->parent->getDevice()->DrawIndexed(3, (UINT)(model.start_index + start_index), (UINT)model.index_shift);
		}
		/*
			Caution: vertex and index buffers will be stored also in system memory.
		*/
		void allowCPUWriteAccess(){
			this->allow_writing = true;
		}
		void send(){
			HRESULT hr = S_OK;
			D3D10_BUFFER_DESC bd;
			bd.ByteWidth = (UINT)(sizeof(T) * this->vertex_buffer_size);
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			if(this->allow_writing) {
				bd.Usage = D3D10_USAGE_DYNAMIC;
				bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			} else {
				bd.Usage = D3D10_USAGE_DEFAULT;
				bd.CPUAccessFlags = 0;
			}
			bd.MiscFlags = 0;
			D3D10_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = this->vertex_buffer_data;
			if(FAILED(this->parent->getDevice()->CreateBuffer(&bd, &InitData, &this->vertex_buffer)))
				return;

			D3D10_SUBRESOURCE_DATA data;
			bd.Usage = D3D10_USAGE_DEFAULT;
			bd.ByteWidth = (UINT)(sizeof(DWORD) * this->index_buffer_size);
			bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			data.pSysMem = this->index_buffer_data;
			hr = this->parent->getDevice()->CreateBuffer(&bd, &data, &this->index_buffer);
			if(FAILED(hr))
				return;

			if(!this->allow_writing){
				delete this->vertex_buffer_data;
				delete this->index_buffer_data;
				this->vertex_buffer_data = _n;
				this->index_buffer_data = _n;
			}
			if(this->debug_mode){
				HRESULT hr = S_OK;
				D3D10_BUFFER_DESC bd;
				bd.ByteWidth = (UINT)(this->normals.size() * 2 * sizeof(T));
				bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
				if(this->allow_writing) {
					bd.Usage = D3D10_USAGE_DYNAMIC;
					bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
				} else {
					bd.Usage = D3D10_USAGE_DEFAULT;
					bd.CPUAccessFlags = 0;
				}
				bd.MiscFlags = 0;
				D3D10_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = this->buildDebugData();
				if(FAILED(this->parent->getDevice()->CreateBuffer(&bd, &InitData, &this->debug_buffer)))
					return;
				UINT stride = sizeof(T);
				UINT offset = 0;
				this->parent->getDevice()->IASetVertexBuffers(1, 1, &this->debug_buffer, &stride, &offset);
			}
			this->already_send = true;
		}
		void release(){
			if(this->vertex_buffer != _n){
				this->vertex_buffer->Release();
			}
			if(this->index_buffer != _n){
				this->index_buffer->Release();
			}
			if(this->index_buffer_data != _n){
				delete this->index_buffer_data;
			}
			if(this->updating_buffer != _n){
				delete this->updating_buffer;
			}
			if(this->vertex_buffer_data != _n){
				delete this->vertex_buffer_data;
			}
		}
		void inverse_normals(_ object){
			model_info current = this->models[object];
			_ iv = current.index_shift;
			_ end_vertex = current.index_shift + current.vertices_number;
			while(iv < end_vertex){
				this->vertex_buffer_data[iv].normal = -this->vertex_buffer_data[iv].normal;
				iv++;
			}
		}
		void activate(){
			UINT stride = sizeof(T);
			UINT offset = 0;
			this->parent->getDevice()->IASetVertexBuffers(0, 1, &this->vertex_buffer, &stride, &offset);
			this->parent->getDevice()->IASetIndexBuffer(this->index_buffer, DXGI_FORMAT_R32_UINT, 0);
		}
		_ freeze(){
			Logger::Main()->log(L"Freeze\n");
			if(flexible){
				assert(this->vertex_buffer_size_limit >= this->current_vertex);
				assert(this->index_buffer_size_limit >= this->current_index);
				this->vertex_buffer_size = this->current_vertex;
				this->index_buffer_size = this->current_index;
				this->vertex_buffer_data = (T*) realloc(this->vertex_buffer_data, sizeof(T) * this->current_vertex);
				this->index_buffer_data = (_d*) realloc(this->index_buffer_data, sizeof(_d) * this->current_index);
				this->vertex_buffer_size = this->vertex_buffer_size_limit = this->current_vertex;
				this->index_buffer_size = this->index_buffer_size_limit = this->current_index;
			}
			if(this->models.size() != 0){
				this->last_model = this->models[this->models.size() - 1];
			}
			model_info current;
			current.start_index = this->last_model.indices_number + this->last_model.start_index;
			current.indices_number = this->index_buffer_size - current.start_index;
			current.index_shift = this->last_model.index_shift + this->last_model.vertices_number;
			current.vertices_number = this->vertex_buffer_size - current.index_shift;
			this->models.push_back(current);
			this->last_model = current;
			return this->models.size() - 1;
		}
		_ freezeTransparent(TranslucencyManager < T > *manager, float opaque){
			_ result = this->freeze();
			_ last_model_id = this->models.size() - 1;
			manager->registerTransparentObject(last_model_id, last_model.start_index, last_model.indices_number, last_model.index_shift, opaque);
			_vector3 current = _vector3(.0f, .0f, .0f);
			for(_ i = 0; i < this->last_model.indices_number; i += 3){
				current = _vector3(.0f, .0f, .0f);
				current += this->getVertex(i, last_model_id).position;
				current += this->getVertex(i + 1, last_model_id).position;
				current += this->getVertex(i + 2, last_model_id).position;
				current /= 3.0f;
				manager->insertTransparentTriangle(last_model_id, current, i);
			}
			return result;
		}
		/*
			Following methods return valid result only after send method call
		*/
		_buffer getIndexBuffer(){
			return this->index_buffer;
		}
		_buffer getVertexBuffer(){
			return this->vertex_buffer;
		}
		/*
			Is it good idea to implement such functionality here?
		*/
		//void accumulate_update(unsigned int object, T *input, _ vertex_first, _ vertex_count);
		/*
			Caution: we can't change topology of object
		*/
		void cumulative_update(_ object, T *input){
			model_info current = this->models[object];
			T *pData = NULL;
			HRESULT result = this->vertex_buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >(&pData));
			if(SUCCEEDED(result))
			{
				memcpy(pData + updating_vertex_start, input, sizeof(T) * updating_vertices_count);
				this->vertex_buffer->Unmap();
				this->updating_buffer = _n;
			}
		}
		/*
			Info: one unused parameter - object id.
		*/
		void set_updating_vertices(_ object, T *input, _ vertex_first, _ vertex_count){
			this->updating_buffer = input;
			this->updating_vertices_count = vertex_count;
			this->updating_vertex_start = vertex_first;
		}
		T* getUnsafeAccess(_ object){
			model_info current = this->models[object];
			return &this->vertex_buffer_data[current.index_shift];
		}
		void vacuum(){
			T *pData = NULL;
			HRESULT result = this->vertex_buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, reinterpret_cast< void** >(&pData));
			if(SUCCEEDED(result))
			{
				memcpy(pData, this->vertex_buffer_data, sizeof(T) * this->vertex_buffer_size);
				this->vertex_buffer->Unmap();
			}
		}
		/*
			Info: not yet implemented.
		*/
		void toggleDebugMode(){
			this->debug_mode = !this->debug_mode;
		}
		void draw_debug_data(){
			this->parent->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			this->parent->getDevice()->Draw(0, this->normals.size() );
		}
		void toggleCompressMode(bool input){
			this->compress_mode = input;
			if(!this->compress_mode){
				this->compressor->clear();
			}
		}
		/*
			Info: only for VertexCompressor
			Caution: this method is good only when resizeInternalBuffers is used.
		*/
		T *getLastModelVertex(_ id){
			return this->vertex_buffer_data + id + this->last_model.index_shift + this->last_model.vertices_number;
		}
	private:
		VertexCompressor < T > *compressor;
		T *buildDebugData(){
			if(this->debug_buffer_data != _n)
				return this->debug_buffer_data;
			this->debug_buffer_data = new T[2 * this->normals.size()];
			_ curr = 0;
			for(_ i = 0; i < this->normals.size(); i++){
				this->debug_buffer_data[curr].normal = _vector3(.0f, .0f, -1.0f);
				this->debug_buffer_data[curr++].position = this->normals[i].from;
				this->debug_buffer_data[curr].normal = _vector3(.0f, .0f, -1.0f);
				this->debug_buffer_data[curr++].position = this->normals[i].to;
			}
			return this->debug_buffer_data;
		}
		T *debug_buffer_data;
		_buffer debug_buffer;
		std::vector < _line > normals;
		bool allow_writing, normal_vertex_order;
		bool exists(_ number, _ object){
			model_info current = this->models[object];
			_ index_number = this->index_buffer_data[number + current.start_index] + current.index_shift - this->updating_vertex_start;
			if(this->updating_buffer != _n){
				return !(index_number < 0 && index_number >= this->updating_vertices_count);
			}
			return false;
		}
		struct model_info{
			//Model stored in index buffer from start_index index to index + indices_number.
			//All indices will be transformed using index_shift field.
			//Model consists of vertices_number vertices, stored in vertex buffer.
			_ index_shift;
			_ start_index;
			_ indices_number;
			_ vertices_number;
			model_info() : start_index(0), indices_number(0), index_shift(0), vertices_number(0) {}
		};
		bool correction_mode;
		bool already_send;
		bool compress_mode;
		T *updating_buffer;
		_ vertex_buffer_size_limit, index_buffer_size_limit;
		_ updating_vertex_start, updating_vertices_count;
		static const _ vertex_buffer_piece;
		static const _ index_buffer_piece;
		bool flexible;
		model_info last_model;
		std::vector < model_info > models;
		bool checkForDuplication(_vector3 &first, _vector3 &second){
			if(first == _vector3(.0f, .0f, .0f)){
				return false;
			}
			float test = D3DXVec3Dot(&first, &second);
			test /= D3DXVec3Length(&second);
			if(test >= sqrtf(2.0f) / 2.0f){
				return false;
			}
			return true;
		}
		_buffer index_buffer;
		_buffer vertex_buffer;
		//absolute vertex number
		T& getVertex(_ number){
			return this->vertex_buffer_data[this->index_buffer_data[number]];
		}
		//relative vertex number
		T& getVertex(_ number, _ object){
			model_info current = this->models[object];
			_ index_number = this->index_buffer_data[number + current.start_index];
			if(this->already_send && this->updating_buffer != _n){
				return this->updating_buffer[index_number + current.index_shift - this->updating_vertex_start];
			}
			return this->vertex_buffer_data[index_number + current.index_shift];
		}
		Artist *parent;
		T *vertex_buffer_data;
		_d *index_buffer_data;
		_ vertex_buffer_size, index_buffer_size, current_vertex, current_index;
		bool debug_mode;
	};
}

#endif