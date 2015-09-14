#ifndef __scene
#define __scene

#include "common.h"
#include "Artist.h"
#include "d3d10_1.h"
#include "d3dx10.h"
#include "Window.h"
#include "Landscape.h"
#include "Effect.h"
#include "LightManager.h"

namespace engine{
	class Camera;
	class Window;

	class Scene{
	public:
		Scene();
		virtual ~Scene();
		virtual void render() = 0;
		virtual void setup(Artist *input) = 0;
		virtual void key_checker(Window *parent) = 0;
		virtual void release() = 0;
		virtual void mouse_checker(_wparam wParam, int x, int y) = 0;
	protected:
		_matrix projection_matrix;
		_matrix camera_matrix;
		Artist *artist;
		Camera *camera;
	};

	class Camera {
	public:
		void set(_vector3 position, _vector3 direction, _vector3 up);
		void move(_vector3 translate);
		_matrix calculateTransformation();
		_vector3 getPosition();
		_vector3 getDirection();
		_vector3 getUp();
		_vector3 getRight();
		Camera();
		void rotateAroundUpVector(float angle);
		void rotateAroundRightVector(float angle);
	private:
		_vector3 position;
		_vector3 direction;
		_vector3 up;
		_vector3 right;
	};
}
#endif