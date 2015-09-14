#ifndef __gpu_calculator
#define __gpu_calculator

#include "common.h"
#include "d3d10_1.h"
#include "Artist.h"

#include <cuda.h>
#include <builtin_types.h>
#include <cuda_runtime_api.h>
#include <cuda_d3d10_interop.h>

namespace engine{
	class gpuCalculator{
	public:
		gpuCalculator(Artist *artist);
		/*
			Due to performance aspects we use one solid vertex and index buffer.
			In main application we can use one buffer for rendering only if we place all translucency objects in the end of the buffers.
		*/
		void registerVertexBuffer(_buffer buffer);
		void registerIndexBuffer(_buffer buffer);
		_ registerObject(_ start_index, _ indices_count, _ index_shift);
		/*
			Perform sorting for all registered transparent objects.
			We need to do this operation in two passes:
			1. Transform all object triangles and saving their centers in additional buffer.
			2. Using additional buffer and current camera position we must sort all geometry and sort indices for all transparent objects.
		*/
		void prepareGPU();
		void release();
		void setTransformation(_ object, _matrix transformation);
		void setCameraPosition(_vector3 &position);
	protected:
		_matrix transformation;
		/*
			256 transparent objects is supported
		*/
		uint3 objects[256];
		float *transformations;
		uint1 object_counter;
		_vector3 camera_position;
		Artist *artist;
		_buffer input_buffers[2];
		bool searchForGPU();
	};
}

#endif