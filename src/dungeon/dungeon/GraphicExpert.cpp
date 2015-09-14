#include "GraphicExpert.h"
#include "common.h"
#include "SimplexNoise.h"
#include <cmath>
#include <stdio.h>
#include <windows.h>

namespace engine{
	_vector3 GraphicExpert::calculate_normal(float *data, _ x, _ y, _ z, _ size){
		_vector3 temp = _vector3(.0f, .0f, .0f);
		_ alpha = get_index(size, x, y, z, true);
		float first = data[alpha];
		_vector3 result = _vector3(.0f, .0f, 1.0f);
		_vector3 normals[3][3][3] = {_vector3(.0f, .0f, .0f)};
		eplfor(a, 3){
			eplfor(b, 3){
				eplfor(c, 3){
					temp = get_normal(a, b, c);
					D3DXVec3Normalize(&temp, &temp);
					normals[a][b][c] = temp;
				}
			}
		}
		eplfor(a, 3){
			eplfor(b, 3){
				eplfor(c, 3){
					if(a == 1 && b == 1 && c == 1){
						continue;
					}
					_ beta = get_index(size, get_near(size, x, a), get_near(size, y, b), get_near(size, z, c), true);
					float second = data[beta];
					result += (second - first) * normals[a][b][c];
				}
			}
		}
		D3DXVec3Normalize(&result, &result);
		return result;
	}
	float* GraphicExpert::createNoiseTexture(_ size, bool normal_map){
		SimplexNoise::init();
		if(normal_map){
			float *result = new float[size * size * size * 3];
			eplfor(i, size){
				eplfor(j, size){
					eplfor(k, size){
						double x = (double) i;
						double y = (double) j;
						double z = (double) k;
						_ base = get_index(size, i, j, k, normal_map);
						result[base] = (float)SimplexNoise::noise(x, y, z);
					}
				}
			}

			eplfor(i, size){
				eplfor(j, size){
					eplfor(k, size){
						double x = (double) i;
						double y = (double) j;
						double z = (double) k;
						_ base = get_index(size, i, j, k, normal_map);
						_vector3 normal = calculate_normal(result, i, j, k, size);
						result[base + 1] = normal.x;
						result[base + 2] = normal.z;
					}
				}
			}
			return result;
		} else {
			float *result = new float[size * size * size];
			eplfor(i, size){
				eplfor(j, size){
					eplfor(k, size){
						double x = (double) i;
						double y = (double) j;
						double z = (double) k;
						result[get_index(size, i, j, k, normal_map)] = (float)SimplexNoise::noise(x, y, z);
					}
				}
			}
			return result;
		}
	}
}