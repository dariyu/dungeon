#include "gpuCalculator.h"
#include "common.h"
#include <cuda.h>
#include <builtin_types.h>
#include "string.h"

namespace engine{
	gpuCalculator::gpuCalculator(Artist *artist){
		cudaError_t result = cudaSuccess;
		this->artist = artist;
		if(this->searchForGPU()){
			result = cudaD3D10SetDirect3DDevice(artist->getDevice());
		}
		this->input_buffers[0] = _n;
		this->input_buffers[1] = _n;
		this->camera_position = _vector3(.0f, .0f, .0f);
		ZeroMemory(this->objects, sizeof(uint3) * 256);
		this->object_counter.x = 0;
		this->transformations = (float*)malloc(sizeof(float) * 4 * 4 * 256);
	}
	_ gpuCalculator::registerObject(_ start_index, _ indices_count, _ index_shift){
		this->objects[this->object_counter.x].x = start_index;
		this->objects[this->object_counter.x].y = indices_count;
		this->objects[this->object_counter.x].z = index_shift;
		return this->object_counter.x++;
	}
	bool gpuCalculator::searchForGPU(){
		char name[256] = {0};
		cudaDeviceProp properties;
		cudaGetDeviceProperties(&properties, 0);
		strcpy(name, properties.name);
		return true;
	}
	void gpuCalculator::release(){
		cudaThreadExit();
	}
	void gpuCalculator::registerVertexBuffer(_buffer buffer){
		this->input_buffers[0] = buffer;
		cudaD3D10RegisterResource(this->input_buffers[0], cudaD3D10RegisterFlagsNone);
		cudaD3D10ResourceSetMapFlags(this->input_buffers[0], cudaD3D10MapFlagsNone);
	}
	void gpuCalculator::registerIndexBuffer(_buffer buffer){
		cudaError_t result = cudaSuccess;
		this->input_buffers[1] = buffer;
		result = cudaD3D10RegisterResource(this->input_buffers[1], cudaD3D10RegisterFlagsNone);
		result = cudaD3D10ResourceSetMapFlags(this->input_buffers[1], cudaD3D10MapFlagsNone);
	}
	void gpuCalculator::setCameraPosition(_vector3 &position){
		this->camera_position = position;
	}
	void gpuCalculator::setTransformation(_ object, _matrix transformation){
		memcpy((void*)(this->transformations + 16 * object), (void*)((float*)transformation), sizeof(float) * 16);
	}
	void gpuCalculator::prepareGPU(){
		/*
			Important : aligned data type is only way for CUDA.
		*/
		cudaError_t result = cudaSuccess;
		float *vertex_buffer = _n, *index_buffer = _n;
		float3 sorting_argument;

		sorting_argument.x = camera_position.x;
		sorting_argument.y = camera_position.y;
		sorting_argument.z = camera_position.z;

		result = cudaD3D10MapResources(2, (ID3D10Resource **)&this->input_buffers[0]);
		//result = cudaD3D10MapResources(1, (ID3D10Resource **)&this->input_buffers[1]);
		result = cudaD3D10ResourceGetMappedPointer( (void **)&vertex_buffer, this->input_buffers[0], 0);
		result = cudaD3D10ResourceGetMappedPointer( (void **)&index_buffer, this->input_buffers[1], 0);

		sort_faces(vertex_buffer, index_buffer, sorting_argument, this->objects, this->object_counter, this->transformations);

		result = cudaD3D10UnmapResources(1, (ID3D10Resource **)&this->input_buffers[0]);
		//result = cudaD3D10UnmapResources(1, (ID3D10Resource **)&this->input_buffers[1]);
	}
}