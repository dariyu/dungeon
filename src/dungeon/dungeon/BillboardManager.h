#ifndef __billboard__
#define __billboard__
#include "common.h"
#include "Effect.h"
#include "Vertices.h"
#include <vector>

namespace engine{
	struct Billboard{
		enum BillboardType{
			Empty = 0,
			Sphere = 1
		};
		_vector3 position;
		_vector4 color;
		float radius;
		BillboardType type;
	};
	class BillboardManager{
	public:
		struct Point{
			_vector3 center;
			_ parent;
			inline Point() : center(_vector3(.0f, .0f, .0f)), parent(0) {}
			inline Point(_vector3 &center, _ parent) : center(center), parent(parent) {}
		};
		BillboardManager(Artist *artist, Effect *effect);
		~BillboardManager();
		void resizeInternalBuffer(_ size);
		void insertBillboard(Billboard &input);
		void draw();
		void send();
		void accumulate();
		void sort();
		void setupCamera(_vector3 position);
		void activate();
		void allowWriting();
		void toggleParticleMode(bool input);
	protected:
		static int point_compare(const void *first, const void *second);
		std::vector < Point > points;
		Point **cache, *sorting, **sorting_buffer;
		static _vector3 camera_position;
		bool allow_writing;
		bool particle_mode;
		Effect *main_effect;
		Artist *artist;
		billboard_vertex *vertices;
		_buffer vertex_buffer;
		_buffer index_buffer;
		_ vertices_limit;
		_ vertices_count;
		_ main_technique, particle_technique;
	};
}

#endif