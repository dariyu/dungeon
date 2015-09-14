#include "d3d10_1.h"
#include "d3dx10.h"
#include "common.h"
#include "Scene.h"

namespace engine{
	_matrix Camera::calculateTransformation(){
		_matrix result;
		D3DXMatrixIdentity(&result);
		_vector3 at = this->direction + this->position;
		D3DXMatrixLookAtLH(&result, &this->position, &at, &this->up);
		return result;
	}
	void Camera::set(_vector3 position, _vector3 direction, _vector3 up){
		this->position = position;
		this->direction = direction;
		this->up = up;
	}
	Camera::Camera(){
		this->up = _vector3(.0f, 1.0f, .0f);
		this->position = _vector3(.0f, .0f, -2.0f);
		this->direction = _vector3(.0f, .0f, 1.0f);
		D3DXVec3Cross(&this->right, &this->direction, &this->up);
	}
	void Camera::move(_vector3 translate){
		this->position += -translate.x * this->right + translate.y * this->up + translate.z * this->direction;
	}
	_vector3 Camera::getPosition(){
		return this->position;
	}
	_vector3 Camera::getDirection(){
		return this->direction;
	}
	_vector3 Camera::getUp(){
		return this->up;
	}
	_vector3 Camera::getRight(){
		return this->right;
	}
	void Camera::rotateAroundUpVector(float angle){
		_vector4 result;
		_matrix transformation;
		D3DXMatrixIdentity(&transformation);
		D3DXMatrixRotationAxis(&transformation, &this->up, angle);
		D3DXVec3Transform(&result, &this->direction, &transformation);
		this->direction.x = result.x;
		this->direction.y = result.y;
		this->direction.z = result.z;
		D3DXVec3Cross(&this->right, &this->direction, &this->up);
	}
	void Camera::rotateAroundRightVector(float angle){
		_vector4 result;
		_matrix transformation;
		D3DXMatrixIdentity(&transformation);
		D3DXMatrixRotationAxis(&transformation, &this->right, angle);
		D3DXVec3Transform(&result, &this->up, &transformation);
		this->up.x = result.x;
		this->up.y = result.y;
		this->up.z = result.z;
		D3DXVec3Cross(&this->direction, &this->up, &this->right);
	}
}