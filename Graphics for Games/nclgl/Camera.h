/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Spline.h"

class Camera	{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
		generatePaths();
	};

	Camera(float pitch, float yaw, Vector3 position){
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
		generatePaths();
	}

	~Camera(void) { delete cameraPath; delete rotationPath; };

	void UpdateCamera(float msec = 10.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	const Vector3& GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees 
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	void ToggleAutomated() { automated = !automated; }

	float GetPathNodeCount() { return cameraPath->GetNodeCount(); }

	void MoveCamera(float count) { position = cameraPath->Next(count); Vector3 direction = rotationPath->Next(count); pitch = direction.x; yaw = direction.y; }

	bool IsAutomated() { return automated; }

protected:
	float	yaw;
	float	pitch;
	Vector3 position;

	bool automated = true;
	Spline* cameraPath;
	Spline* rotationPath;
	float cameraSpeed = 200.0f;

	void generatePaths();
};