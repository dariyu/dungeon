#ifndef __weaver
#define __weaver
#include "Artist.h"
#include "VertexObject.h"

namespace engine{
	/*
		Utility class for creating advanced CPU-sided geometry objects representations.
	*/
	template < class T = engine::simple_vertex >
	class Weaver{
	public:
		enum _mode{
			Triangles,
			Quads,
			Patches
		};
		Weaver(VertexObject < T > *parent){
			this->parent = parent;
			this->mode = Weaver::Triangles;
			this->counter = 0;
			this->spline_detalization = 3;
			this->current_patch_indices = _n;
		}
		void begin(_mode mode){
			this->mode = mode;
			this->counter = 0;
			this->index_cache.clear();
			this->vertex_cache.clear();
			this->vertex_id.clear();
		}
		_ insert_vertex(simple_vertex input){
			_ number = 0;
			switch(this->mode){
				case Weaver::Triangles:
					this->index_cache.push_back(this->parent->insert_vertex(input));
					/*
						We don't use any caching technique in this mode because of simplicity primitive building method.
					*/
					if(this->counter == 3){
						this->parent->insert_triangle(this->index_cache[0], this->index_cache[1], this->index_cache[2]);
						index_cache.clear();
						this->counter = 0;
					}
					return 0;
				case Weaver::Quads:
					/*
						In this mode we don't produce additional vertices so we can easily add all input vertices into the VertexObject immediately.
					*/
					number = this->parent->insert_vertex(input);
					this->vertex_id.push_back(number);
					return this->vertex_id.size() - 1;
				case Weaver::Patches:
					vertex_cache.push_back(input);
					return vertex_cache.size() - 1;
			}
			return 0;
		}
		/*
			Because of variable primitive complexity I decided to use per-index method rather than several per-primitive methods.
		*/
		void insert_primitive_index(_ index){
			this->index_cache.push_back(index);
		}
		/*
			When end method is called all calculated vertices and indices is send to VertexObject.
		*/
		void end(){
			Logger *logger = Logger::Main();
			_ size = this->spline_detalization == 0 ? 1 : 3 * this->spline_detalization - (this->spline_detalization - 1) - 2;
			_ height = 2 * size + 3;
			_ width = 2 * size + 3;
			_ index_count = 1;
			if(height != 1){
				index_count *= height + (height - 3) / 2;
			} else {
				index_count *= height;
			}
			if(width != 1){
				index_count *= width + (width - 3) / 2;
			} else {
				index_count *= width;
			}
			index_count /= 3;
			index_count *= 8;
			switch(this->mode){
				case Weaver::Quads:
					this->parent->resizeInternalBuffers(0, this->index_cache.size() * 3);
					for(_ i = 0; i < this->index_cache.size(); i += 9){
						for(_ j = 1; j < 8; j++){
							this->parent->insert_triangle(	this->vertex_id[this->index_cache[i]], 
															this->vertex_id[this->index_cache[i + j]], 
															this->vertex_id[this->index_cache[i + j + 1]]);
						}
						this->parent->insert_triangle(	this->vertex_id[this->index_cache[i]], 
														this->vertex_id[this->index_cache[i + 8]], 
														this->vertex_id[this->index_cache[i + 1]]);
					}
					break;
				case Weaver::Patches:
					size = 3 + 2 * size;
					this->parent->resizeInternalBuffers(size * size, index_count * this->index_cache.size() / 9);
					this->parent->toggleCompressMode(true);
					for(_ i = 0; i < this->index_cache.size(); i += 9){
						logger->log(L"Patch:\n");
						logger->add();
						this->build_patch(i);
						logger->sub();
						logger->log(L"\n");
					}
					this->parent->toggleCompressMode(false);
					break;
			}
			this->index_cache.clear();
			this->vertex_cache.clear();
			this->vertex_id.clear();
		}
	private:
		_ getIndex(_ x, _ y){
			return this->current_patch_indices[x][y];
		}
		void build_patch(_ starting_index){
			_ size = this->spline_detalization == 0 ? 0 : 3 * this->spline_detalization - (this->spline_detalization - 1) - 2;
			_ border = 2 * size + 3;
			simple_vertex **result = new simple_vertex*[border];
			simple_vertex **initial = new simple_vertex*[3];
			simple_vertex data[3][3];
			this->current_patch_indices = new _*[border];
			data[0][0] = this->vertex_cache[this->index_cache[starting_index + 2]];
			data[0][1] = this->vertex_cache[this->index_cache[starting_index + 3]];
			data[0][2] = this->vertex_cache[this->index_cache[starting_index + 4]];

			data[1][0] = this->vertex_cache[this->index_cache[starting_index + 1]];
			data[1][1] = this->vertex_cache[this->index_cache[starting_index + 0]];
			data[1][2] = this->vertex_cache[this->index_cache[starting_index + 5]];

			data[2][0] = this->vertex_cache[this->index_cache[starting_index + 8]];
			data[2][1] = this->vertex_cache[this->index_cache[starting_index + 7]];
			data[2][2] = this->vertex_cache[this->index_cache[starting_index + 6]];

			//equal with result[i][0]
			initial[0] = this->build_spline(border, data[0][0], data[1][0], data[2][0]);
			//equal with result[i][size + 1]
			initial[1] = this->build_spline(border, data[0][1], data[1][1], data[2][1]);
			//equal with result[i][2 * size + 2]
			initial[2] = this->build_spline(border, data[0][2], data[1][2], data[2][2]);
			for(_ i = 0; i < border; i++){
				result[i] = this->build_spline(border, initial[0][i], initial[1][i], initial[2][i]);
				this->current_patch_indices[i] = new _[border];
			}
			/*
				Push all required vertex data to VertexObject.
				I don't want to manually remove duplicated vertices, so I enable compress mode on VertexObject side.
			*/
			for(_ i = 0; i < border; i++){
				for(_ j = 0; j < border; j++){
					this->current_patch_indices[i][j] = this->parent->insert_vertex(result[i][j]);
				}
			}

			for(_ x = 1; x < border; x += 2){
				for(_ y = 1; y < border; y += 2){
					this->parent->insert_triangle(this->getIndex(x, y + 1), this->getIndex(x, y), this->getIndex(x - 1, y + 1));
					this->parent->insert_triangle(this->getIndex(x - 1, y + 1), this->getIndex(x, y), this->getIndex(x - 1, y));

					this->parent->insert_triangle(this->getIndex(x - 1, y), this->getIndex(x, y), this->getIndex(x - 1, y - 1));
					this->parent->insert_triangle(this->getIndex(x - 1, y - 1), this->getIndex(x, y), this->getIndex(x, y - 1));

					this->parent->insert_triangle(this->getIndex(x, y - 1), this->getIndex(x, y), this->getIndex(x + 1, y - 1));
					this->parent->insert_triangle(this->getIndex(x + 1, y - 1), this->getIndex(x, y), this->getIndex(x + 1, y));

					this->parent->insert_triangle(this->getIndex(x + 1, y), this->getIndex(x, y), this->getIndex(x + 1, y + 1));
					this->parent->insert_triangle(this->getIndex(x + 1, y + 1), this->getIndex(x, y), this->getIndex(x, y + 1));
				}
			}

			for(_ i = 0; i < border; i++){
				delete result[i];
				delete this->current_patch_indices[i];
			}
			for(_ i = 0; i < 3; i++){
				delete initial[i];
			}
			delete this->current_patch_indices;
			delete result;
			delete initial;
			this->current_patch_indices = _n;
		}
		simple_vertex *build_spline(_ number, simple_vertex &alpha, simple_vertex &beta, simple_vertex &gamma){
			simple_vertex *result = new simple_vertex[number];
			_ curr = 0;
			_vector3 a, t;
			a = (alpha.position + beta.position * 2.0f) / 3.0f;
			number -= 3;
			number /= 2;
			result[curr++] = alpha;
			for(_ i = 0; i < number; i++){
				float time = ((float)i + 1.0f) / (2.0f * (float)number + 2.0f) / 2.0f;
				//t = (1.0f - 2.0f * time) * alpha.position + (2.0f * time) * beta.position;
				D3DXVec3CatmullRom(&t, &beta.position, &alpha.position, &beta.position, &gamma.position, time);
				result[curr++].position = t;
			}
			result[curr++] = beta;
			for(_ i = 0; i < number; i++){
				float time = ((float)i + 1.0f) / (2.0f * (float)number + 2.0f) / 2.0f;
				//t = (1.0f - 2.0f * time) * beta.position + (2.0f * time) * gamma.position;
				D3DXVec3CatmullRom(&t, &alpha.position, &beta.position, &gamma.position, &beta.position, time);
				result[curr++].position = t;
			}
			result[curr++] = gamma;
			return result;
		}
		_mode mode;
		_ counter;
		_ spline_detalization;
		std::vector < simple_vertex > vertex_cache;
		std::vector < _ > index_cache;
		std::vector < _ > vertex_id;
		VertexObject < T > *parent;
		_ **current_patch_indices;
	};
	class Smoother{
	public:
		Smoother();
		void setSmoothFactor(_ factor);
		float *smoothHeightMap(float *data, _ width, _ height);
		_ getWidth();
		_ getHeight();
	private:
		void build_row_spline(_ row_number, _ input_row_number);
		void build_column_spline(_ column_number);
		_ getIndex(_ x, _ y);
		_ getInputIndex(_ x, _ y);
		_vector3 getInputVertex(_ x, _ y);
		_vector3 getVertex(_ x, _ y);
		_ getRowNumber(_ fake_number);
		_ smooth_factor;
		_ width;
		_ height;
		_ smooth_width;
		_ smooth_height;
		float *input_data;
		float *result_data;
	};
}
#endif