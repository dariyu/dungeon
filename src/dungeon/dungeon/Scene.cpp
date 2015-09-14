#include "scene.h"
#include "Artist.h"
#include "d3d10_1.h"
#include "common.h"
#include "Primitives.h"
#include "Landscape.h"
#include "Effect.h"

namespace engine{
	Scene::~Scene(){
		delete camera;
	}
	Scene::Scene(){
		this->artist = _n;
		this->camera = new Camera();
		D3DXMatrixIdentity(&this->projection_matrix);
		D3DXMatrixIdentity(&this->camera_matrix);
	}
}