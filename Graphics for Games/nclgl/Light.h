//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
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

	Light(Vector3 position, Vector3 lookAt, Matrix4 projectionMatrix, Vector4 colour, float radius)
	{
		this->position = position;
		this->lookAt = lookAt;
		this->projMatrix = projectionMatrix;
		this->colour = colour;
		this->radius = radius;
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

protected:
	Matrix4 projMatrix;
	Vector3 position;
	Vector3 lookAt;
	Vector4 colour;
	float radius;
};
