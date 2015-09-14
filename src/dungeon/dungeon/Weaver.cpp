#include "Weaver.h"
#include "Artist.h"
#include "d3d10_1.h"
#include "d3dx10.h"
#include "Logger.h"

namespace engine{
	Smoother::Smoother(){
		this->smooth_factor = 1;
		this->height = 0;
		this->width = 0;
		this->smooth_height = 0;
		this->smooth_width = 0;
	}
	void Smoother::setSmoothFactor(_ factor){
		this->smooth_factor = factor;
	}
	float *Smoother::smoothHeightMap(float *data, _ width, _ height){
		this->input_data = data;
		this->width = width;
		this->height = height;
		this->smooth_width = width + (width - 1) * this->smooth_factor;
		this->smooth_height = height + (height - 1) * this->smooth_factor;
		this->result_data = (float*)malloc(sizeof(float) * this->smooth_width * this->smooth_height);
		_ curr = 0;
		_ curr_row = 0;
		eplfor(i, this->width){
			_ curr_column = 0;
			eplfor(j, this->height){
				this->result_data[this->getIndex(curr_row, curr_column)] = this->input_data[this->getInputIndex(i, j)];
				curr_column += this->smooth_factor + 1;
			}
			curr_row += this->smooth_factor + 1;
		}
		eplfor(i, this->width){
			this->build_row_spline(curr, i);
			curr += this->smooth_factor + 1;
		}
		eplfor(j, this->smooth_height){
			this->build_column_spline(j);
		}
		return this->result_data;
	}
	void Smoother::build_row_spline(_ row_number, _ input_row_number){
		_ curr = 1;
		_vector3 t;
		_vector3 border_vector;
		float time = .0f;
		border_vector = this->getInputVertex(input_row_number, 0) * 2.0f - this->getInputVertex(input_row_number, 1);
		eplfor(j, this->smooth_factor){
			time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
			D3DXVec3CatmullRom(&t,	&border_vector, 
									&this->getInputVertex(input_row_number, 0), 
									&this->getInputVertex(input_row_number, 1), 
									&this->getInputVertex(input_row_number, 2),
									time);
			this->result_data[this->getIndex(row_number, curr++)] = t.y;
		}
		curr++;
		eplfor(i, this->height - 3){
			eplfor(j, this->smooth_factor){
				time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
				D3DXVec3CatmullRom(&t,	&this->getInputVertex(input_row_number, i), 
										&this->getInputVertex(input_row_number, i + 1), 
										&this->getInputVertex(input_row_number, i + 2), 
										&this->getInputVertex(input_row_number, i + 3),
										time);
				this->result_data[this->getIndex(row_number, curr++)] = t.y;
			}
			curr++;
		}
		border_vector = this->getInputVertex(input_row_number, this->height - 1) * 2.0f - this->getInputVertex(input_row_number, this->height - 2);
		eplfor(j, this->smooth_factor){
			time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
			D3DXVec3CatmullRom(&t,	&this->getInputVertex(input_row_number, this->height - 3), 
									&this->getInputVertex(input_row_number, this->height - 2), 
									&this->getInputVertex(input_row_number, this->height - 1), 
									&border_vector,
									time);
			this->result_data[this->getIndex(row_number, curr++)] = t.y;
		}
	}
	void Smoother::build_column_spline(_ column_number){
		_ curr = 1;
		_vector3 t;
		_vector3 border_vector;
		float time = .0f;
		border_vector = this->getVertex(this->getRowNumber(0), column_number) * 2.0f - this->getVertex(this->getRowNumber(1), column_number);
		eplfor(j, this->smooth_factor){
			time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
			D3DXVec3CatmullRom(&t,	&border_vector, 
									&this->getVertex(this->getRowNumber(0), column_number), 
									&this->getVertex(this->getRowNumber(1), column_number), 
									&this->getVertex(this->getRowNumber(2), column_number),
									time);
			this->result_data[this->getIndex(curr++, column_number)] = t.y;
		}
		curr++;
		eplfor(i, this->width - 3){
			eplfor(j, this->smooth_factor){
				time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
				D3DXVec3CatmullRom(&t,	&this->getVertex(this->getRowNumber(i), column_number), 
										&this->getVertex(this->getRowNumber(i + 1), column_number), 
										&this->getVertex(this->getRowNumber(i + 2), column_number), 
										&this->getVertex(this->getRowNumber(i + 3), column_number),
										time);
				this->result_data[this->getIndex(curr++, column_number)] = t.y;
			}
			curr++;
		}
		border_vector = this->getVertex(this->getRowNumber(this->width - 1), column_number) * 2.0f - this->getVertex(this->getRowNumber(this->width - 2), column_number);
		eplfor(j, this->smooth_factor){
			time = ((float)j + 1.0f) / ((float)this->smooth_factor + 1.0f);
			D3DXVec3CatmullRom(&t,	&this->getVertex(this->getRowNumber(this->width - 3), column_number), 
									&this->getVertex(this->getRowNumber(this->width - 2), column_number), 
									&this->getVertex(this->getRowNumber(this->width - 1), column_number), 
									&border_vector,
									time);
			this->result_data[this->getIndex(curr++, column_number)] = t.y;
		}
	}
	_vector3 Smoother::getInputVertex(_ x, _ y){
		_vector3 result;
		result.y = this->input_data[this->getInputIndex(x, y)];
		result.x = (float) x / (float) this->width;
		result.z = (float) y / (float) this->height;
		return result;
	}
	_ Smoother::getIndex(_ x, _ y){
		return x * this->smooth_width + y;
	}
	_ Smoother::getInputIndex(_ x, _ y){
		return x * this->width + y;
	}
	_ Smoother::getWidth(){
		return this->smooth_width;
	}
	_ Smoother::getHeight(){
		return this->smooth_height;
	}
	_vector3 Smoother::getVertex(_ x, _ y){
		_vector3 result;
		result.y = this->result_data[this->getIndex(x, y)];
		result.x = (float) x / (float) this->width;
		result.z = (float) y / (float) this->height;
		return result;
	}
	_ Smoother::getRowNumber(_ fake_number){
		return this->smooth_factor * fake_number + fake_number;
	}
}