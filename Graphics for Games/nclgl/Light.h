//Author:			Daniel Cieslowski
//Last Modified:	26/11/2019
//Student No:		190562751
#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light
{
public:
	Light()
	{
		this->position = Vector3(0,0,0);
		this->colour = Vector4(0,0,0,0);
		this->radius = 1000;
		this->lookAt = Vector3(0, 0, 0);
	}

	Light(Vector3 position, Vector3 lookAt, float nearZ, float farZ, float fov, float ratio, Vector4 colour, float radius)
	{
		this->position = position;
		this->lookAt = lookAt;
		this->colour = colour;
		this->radius = radius;
		this->nearZ = nearZ;
		this->farZ = farZ;
		this->fov = fov;
		this->ratio = ratio;
		this->projMatrix = Matrix4::Perspective(nearZ, farZ, ratio, fov);
	}
	
	~Light(void) {};
	
	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	
	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

	Matrix4 GetProjectionMatrix() const { return projMatrix; }
	void SetProjectionMatrix(Matrix4 projection) { projMatrix = projection; }

	Vector3 GetDirection() const { return lookAt; }
	void SetDirection(Vector3 direction) { lookAt = direction; }

	float GetNear() const { return nearZ; }
	float GetFar() const { return farZ; }
	float GetFOV() const { return fov; }
	float GetAspect() const { return ratio; }

protected:
	Matrix4 projMatrix;
	Vector3 position;
	Vector3 lookAt;
	Vector4 colour;
	float radius;
	float nearZ;
	float farZ;
	float fov;
	float ratio;
};
