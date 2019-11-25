//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#pragma once

#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"
class Matrix4;

class Frustum 
{
public:
	Frustum(void) {};
	~Frustum(void) {};
	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(SceneNode& n);
protected:
	Plane planes[6];
};
