#ifndef __landscape
#define __landscape

#include "common.h"
#include "Artist.h"
#include "VertexObject.h"
#include <string>

namespace engine{
	/*
		New generation of high-order vertex objects
		TODO: decide what to do: one object - one vertex buffer or one vertex buffer for each objects
	*/
	class Artist;
	class Landscape{
	public:
		Landscape(Artist *parent, VertexObject < simple_vertex > *input = _n);
		~Landscape();
		void saveToFile(std::wstring filename);
		void loadFromFIle(std::wstring filename);
		void tectonic_step();
		void erosive_step();
		void initial_step();
		void random_step();
		void setSize(_ width, _ height);
		void release();
		void draw();
		void fill();
		void smooth(_ detalization);
		void toggleSplines(bool input);
		_vector3 getVertex(_ x, _ y);
		simple_vertex* build_data();
		_ getIndex(_ x, _ y);
		void sync();
		void linear_step();
	private:
		bool enable_splines;
		void tension_simulator(_ tx, _ ty, _ dx, _ dy, _ force);
		void place_vulcan(_ vx, _ vy, _ force);
		void plasma_generator(_ xa, _ ya, _ xb, _ yb);
		bool shared_vertex_object;
		bool *used;
		_ id;
		Artist *parent;
		VertexObject < simple_vertex > *object;
		_ width, height;
		float *data;
		float *tension;
		_ tectonic_step_number;
		_ erosive_step_number;
	};
}
#endif