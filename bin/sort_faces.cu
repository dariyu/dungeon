#ifndef _SIMPLED3D_KERNEL_CU_
#define _SIMPLED3D_KERNEL_CU_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cuda.h>
#include <builtin_types.h>
#include <cuda_runtime_api.h>

#include <cutil_inline.h>

///////////////////////////////////////////////////////////////////////////////
//! Simple kernel to modify vertex positions in sine wave pattern
//! @param pos  pos in global memory
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel( float4* pos, unsigned int width, unsigned int height, float time)
{
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

    // calculate uv coordinates
    float u = x / (float) width;
    float v = y / (float) height;
    u = u*2.0f - 1.0f;
    v = v*2.0f - 1.0f;

    // calculate simple sine wave pattern
    float freq = 4.0f;
    float w = sinf(u*freq + time) * cosf(v*freq + time) * 0.5f;

    // write output vertex
    pos[y*width+x] = make_float4(u, w, v, __int_as_float(0xff00ff00));
}

extern "C" 
void sort_faces(float* vertex_buffer, float* index_buffer, float3 sorting_argument, uint3 *objects, uint1 count, float* tranformations){
    /*cudaError_t error = cudaSuccess;

    dim3 block(8, 8, 1);
    dim3 grid(width / block.x, height / block.y, 1);

	kernel<<<grid, block>>>(pos, width, height, time);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        printf("kernel() failed to launch error = %d\n", error);
    }*/
}

#endif
