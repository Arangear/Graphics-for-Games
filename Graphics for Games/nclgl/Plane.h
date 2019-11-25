//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#pragma once
#include "vector3.h"

class Plane 
{
public:
	Plane(void) {};
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane(void) {};

	void SetNormal(const Vector3& normal) { this->normal = normal; }
	Vector3 GetNormal() const { return normal; }

	void SetDistance(float dist) { distance = dist; }
	float GetDistance() const { return distance; }

	bool SphereInPlane(const Vector3& position, float radius) const;

 protected:
	Vector3 normal;
	float distance;
};
