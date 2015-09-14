#ifndef __primitives
#define __primitives

#include "common.h"
#include "VertexObject.h"
#include "TranslucencyManager.h"

namespace engine{
	template < class T = engine::simple_vertex >
	class Creator{
	public:
		static void toggleTranslucency(bool state){
			Creator::translucency = state;
			if(!state)
				Creator::opaque = 1.0f;
		}
		static void setColor(_vector4 &input){
			Creator::current_color = input;
		}
		static void setTranslucencyManager(TranslucencyManager < T > *alpha){
			Creator::translucency_manager = alpha;
		}
		static void setOpaque(float opaque){
			Creator::opaque = opaque;
		}
	protected:
		static float opaque;
		static bool translucency;
		static _vector4 current_color;
		static TranslucencyManager < T > *translucency_manager;
	};
	template < class T = engine::simple_vertex >
	class Cube : public Creator < T > {
	public:
		static _ create(VertexObject < T > *cube){
			cube->resizeInternalBuffers(8, 36);

			cube->insert_vertex(simple_vertex(_vector3(-1.0f, 1.0f, -1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(1.0f, 1.0f, -1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(1.0f, 1.0f, 1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(-1.0f, 1.0f, 1.0f)));

			cube->insert_vertex(simple_vertex(_vector3(-1.0f, -1.0f, -1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(1.0f, -1.0f, -1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(1.0f, -1.0f, 1.0f)));
			cube->insert_vertex(simple_vertex(_vector3(-1.0f, -1.0f, 1.0f)));

			cube->insert_triangle(3, 1, 0);
			cube->insert_triangle(2, 1, 3);
			cube->insert_triangle(0, 5, 4);
			cube->insert_triangle(1, 5, 0);

			cube->insert_triangle(3, 4, 7);
			cube->insert_triangle(0, 4, 3);
			cube->insert_triangle(1, 6, 5);
			cube->insert_triangle(2, 6, 1);

			cube->insert_triangle(2, 7, 6);
			cube->insert_triangle(3, 7, 2);
			cube->insert_triangle(6, 4, 5);
			cube->insert_triangle(7, 4, 6);

			_ id = 0;
			if(Creator::translucency){
				id = cube->freezeTransparent(Creator::translucency_manager, Creator::opaque);
			} else {
				id = cube->freeze();
			}
			//cube->fill(id, Creator::current_color);
			cube->advanced_calculate_normals_for_last_model();
			return id;
		}
	};
	template < class T = engine::simple_vertex >
	class Piramide : public Creator < T > {
	public:
		static _ create(VertexObject < T > *piramide){
			piramide->resizeInternalBuffers(5, 18);

			piramide->insert_vertex(simple_vertex(_vector3(-1.0f, -1.0f, -1.0f)));
			piramide->insert_vertex(simple_vertex(_vector3(1.0f, -1.0f, -1.0f)));
			piramide->insert_vertex(simple_vertex(_vector3(1.0f, -1.0f, 1.0f)));
			piramide->insert_vertex(simple_vertex(_vector3(-1.0f, -1.0f, 1.0f)));

			piramide->insert_vertex(simple_vertex(_vector3(.0f, 1.0f, .0f)));

			piramide->insert_triangle(1, 3, 0);
			piramide->insert_triangle(1, 2, 3);

			piramide->insert_triangle(1, 0, 4);
			piramide->insert_triangle(2, 1, 4);
			piramide->insert_triangle(3, 2, 4);
			piramide->insert_triangle(0, 3, 4);

			_ id = 0;
			if(Creator::translucency){
				id = piramide->freezeTransparent(Creator::translucency_manager, Creator::opaque);
			} else {
				id = piramide->freeze();
			}
			//piramide->fill(id, Creator::current_color);
			piramide->advanced_calculate_normals_for_last_model();
			return id;
		}
	};
	template < class T = engine::simple_vertex >
	class Sphere : public Creator < T > {
	public:
		static _ create(VertexObject < T > *sphere, _ det){
			float detalization = float(det);
			float step = (float)D3DX_PI / 2.0f / detalization;
			_ i = 0, j = 0;
			_ debug_counter = 0;
			sphere->resizeInternalBuffers(2 + (2 * det - 1) * 4 * det, 3 * (det == 1 ? 8 : 4 * det * (2 + 2 * (2 * det - 1))));
			for(float beta = .0f; i < 4 * det; beta += step, i++){
				j = 0;
				for(float alpha = .0f; j < 2 * det + 1; alpha += step, j++){
					if(i != 0){
						if(j == 0 || j == 2 * det)
							continue;
					}
					simple_vertex current;
					current.position.x = sinf(alpha) * cosf(beta);
					current.position.y = cosf(alpha);
					current.position.z = sinf(alpha) * sinf(beta);
					sphere->insert_vertex(current);
					debug_counter++;
				}
			}
			debug_counter = 0;
			for(_ i = 0; i < 4 * det; i++){
				sphere->insert_triangle(Sphere::number(i + 1, det, 1), Sphere::number(i, det, 1), Sphere::number(i, det, 0));
				if(det != 1){
					for(_ j = 1; j < 2 * det - 1; j++){
						debug_counter += 6;
						sphere->insert_triangle(Sphere::number(i, det, j), Sphere::number(i + 1, det, j), Sphere::number(i, det, j + 1));
						sphere->insert_triangle(Sphere::number(i + 1, det, j + 1), Sphere::number(i, det, j + 1), Sphere::number(i + 1, det, j));
					}
				}
				sphere->insert_triangle(Sphere::number(i, det, 2 * det - 1), Sphere::number(i + 1, det, 2 * det - 1), Sphere::number(i, det, 2 * det));
				debug_counter += 6;
			}
			_ id = 0;
			if(Creator::translucency){
				id = sphere->freezeTransparent(Creator::translucency_manager, Creator::opaque);
			} else {
				id = sphere->freeze();
			}
			sphere->calculate_normals(id);
			return id;
		}
	protected:
		static _ number(_ slice, _ det, _ number){
			slice = slice % (4 * det);
			if(slice == 0){
				return number;
			} else {
				if(number == 0){
					return 0;
				}
				if(number == 2 * det){
					return 2 * det;
				}
				return slice * (2 * det - 1) + 2 + number - 1;
			}
		}
	};
	template < class T = engine::textured_vertex >
	class Mirror : public Creator < T > {
	public:
		static _ create(VertexObject < T > *mirror){
			_ id = 0;
			mirror->resizeInternalBuffers(4, 6);
			mirror->insert_vertex(textured_vertex(_vector3(-1.0f, -1.0f, .0f), _vector2(1.0f, 1.0f)));
			mirror->insert_vertex(textured_vertex(_vector3(1.0f, -1.0f, .0f), _vector2(.0f, 1.0f)));
			mirror->insert_vertex(textured_vertex(_vector3(1.0f, 1.0f, .0f), _vector2(.0f, .0f)));
			mirror->insert_vertex(textured_vertex(_vector3(-1.0f, 1.0f, .0f), _vector2(1.0f, .0f)));
			mirror->insert_triangle(3, 1, 0);
			mirror->insert_triangle(2, 1, 3);
			mirror->insert_triangle(1, 3, 0);
			mirror->insert_triangle(1, 2, 3);
			id = mirror->freeze();
			mirror->advanced_calculate_normals_for_last_model();
			return id;
		}
	};
	template < class T = engine::simple_vertex >
	class Cylinder : public Creator < T > {
	public:
		static _ create(VertexObject < T > *cylinder, _ det){
			_ id = 0;
			float detalization = float(det);
			float step = (float)D3DX_PI / 2.0f / detalization;
			_ i = 0, j = 0;
			cylinder->resizeInternalBuffers(2 * 4 * det, 4 * det * 2 * 3);
			for(float beta = .0f; i < 4 * det; beta += step, i++){
				simple_vertex current;
				current.position.x = cosf(beta) / 2.0f;
				current.position.y = -.5f;
				current.position.z = sinf(beta) / 2.0f;
				cylinder->insert_vertex(current);
				current.position.y = .5f;
				cylinder->insert_vertex(current);
			}
			for(_ i = 0; i < 4 * det; i++){
				cylinder->insert_triangle(Cylinder::number(i, det, 0), Cylinder::number(i, det, 1), Cylinder::number(i + 1, det, 0));
				cylinder->insert_triangle(Cylinder::number(i, det, 1), Cylinder::number(i + 1, det, 1), Cylinder::number(i + 1, det, 0));
			}
			if(Creator::translucency){
				id = cylinder->freezeTransparent(Creator::translucency_manager, Creator::opaque);
			} else {
				id = cylinder->freeze();
			}
			//cylinder->fill(id, Creator::current_color);
			cylinder->calculate_normals(id);
			return id;
		}
	protected:
		static _ number(_ slice, _ det, _ number){
			slice = slice % (4 * det);
			return number + slice * 2;
		}
	};
	template < class T = engine::simple_vertex >
	class Torus : public Creator < T > {
	public:
		static _ create(VertexObject < T > *torus, _ det){
			_ id = 0;
			float detalization = float(det);
			float step = (float)D3DX_PI / 2.0f / detalization;
			_ i = 0, j = 0;
			torus->resizeInternalBuffers(4 * det * 4 * det, 4 * det * 4 * det * 2 * 3);
			for(float beta = .0f, i = 0; i < 4 * det; beta += step, i++){
				for(float alpha = .0f, j = 0; j < 4 * det; alpha += step, j++){
					simple_vertex current;
					current.position.x = cosf(beta) + cosf(alpha) * .3f;
					current.position.y = sinf(alpha) * .3f;
					current.position.z = sinf(beta);
					torus->insert_vertex(current);
				}
			}
			for(_ i = 0; i < 4 * det; i++){
				for(_ j = 0; j < 4 * det; j++){
					torus->insert_triangle(Torus::number(i, det, j), Torus::number(i + 1, det, j), Torus::number(i, det, j + 1));
					torus->insert_triangle(Torus::number(i + 1, det, j), Torus::number(i + 1, det, j + 1), Torus::number(i, det, j + 1));
				}
			}
			if(Creator::translucency){
				id = torus->freezeTransparent(Creator::translucency_manager, Creator::opaque);
			} else {
				id = torus->freeze();
			}
			//torus->fill(id, Creator::current_color);
			torus->calculate_normals(id);
			return id;
		}
	protected:
		static _ number(_ slice, _ det, _ number){
			slice = slice % (4 * det);
			number = number % (4 * det);
			return slice * 4 * det + number;
		}
	};
}
#endif