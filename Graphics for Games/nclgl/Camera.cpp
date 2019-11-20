#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)	
{
	if (!automated)
	{
		//Update the mouse by how much
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		//Bounds check the pitch, to be between straight up and straight down ;)
		pitch = min(pitch, 90.0f);
		pitch = max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		msec *= 5.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += msec;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= msec;
		}
	}
	else
	{
		position = cameraPath->Next((int)(cameraSpeed * msec / 1000.0f));

		Vector3 direction = rotationPath->Next((int)(cameraSpeed * msec / 1000.0f));

		pitch = direction.x;
		yaw = direction.y;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * 
			Matrix4::Rotation(-yaw, Vector3(0,1,0)) * 
			Matrix4::Translation(-position);
}

void Camera::generatePaths()
{
	cameraPath = new Spline(3200, false);
	rotationPath = new Spline(3200, true);

	cameraPath->AddWaypoint(Vector3(-4000, 5000, 0));
	cameraPath->AddWaypoint(Vector3(-3000, 4000, 1000));
	cameraPath->AddWaypoint(Vector3(-2000, 3000, 2000));
	cameraPath->AddWaypoint(Vector3(-1000, 4000, 3000));
	cameraPath->AddWaypoint(Vector3(0, 5000, 4000));
	cameraPath->AddWaypoint(Vector3(1000, 6000, 3000));
	cameraPath->AddWaypoint(Vector3(2000, 7000, 2000));
	cameraPath->AddWaypoint(Vector3(3000, 6000, 1000));
	cameraPath->AddWaypoint(Vector3(4000, 5000, 0));
	cameraPath->AddWaypoint(Vector3(3000, 4000, -1000));
	cameraPath->AddWaypoint(Vector3(2000, 3000, -2000));
	cameraPath->AddWaypoint(Vector3(1000, 4000, -3000));
	cameraPath->AddWaypoint(Vector3(0, 5000, -4000));
	cameraPath->AddWaypoint(Vector3(-1000, 6000, -3000));
	cameraPath->AddWaypoint(Vector3(-2000, 7000, -2000));
	cameraPath->AddWaypoint(Vector3(-3000, 6000, -1000));

	rotationPath->AddWaypoint(Vector3(-40, -90, 0));
	rotationPath->AddWaypoint(Vector3(-35, -67.5, 0));
	rotationPath->AddWaypoint(Vector3(-30, -45, 0));
	rotationPath->AddWaypoint(Vector3(-35, -22.5, 0));
	rotationPath->AddWaypoint(Vector3(-40, 0, 0));
	rotationPath->AddWaypoint(Vector3(-45, 22.5, 0));
	rotationPath->AddWaypoint(Vector3(-50, 45, 0));
	rotationPath->AddWaypoint(Vector3(-45, 67.5, 0));
	rotationPath->AddWaypoint(Vector3(-40, 90, 0));
	rotationPath->AddWaypoint(Vector3(-35, 112.5, 0));
	rotationPath->AddWaypoint(Vector3(-30, 135, 0));
	rotationPath->AddWaypoint(Vector3(-35, 157.5, 0));
	rotationPath->AddWaypoint(Vector3(-40, 180, 0));
	rotationPath->AddWaypoint(Vector3(-45, 202.5, 0));
	rotationPath->AddWaypoint(Vector3(-50, 225, 0));
	rotationPath->AddWaypoint(Vector3(-45, 247.5, 0));

	cameraPath->GenerateNodes();
	rotationPath->GenerateNodes();
}
