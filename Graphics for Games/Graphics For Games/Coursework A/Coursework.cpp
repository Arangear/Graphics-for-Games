//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main()
{
	Window window("Coursework", 800, 600, false);
	if(!window.HasInitialised())
	{
		return -1;
	}
	
	Renderer renderer(window);
	if(!renderer.HasInitialised())
	{
		system("PAUSE");
		return -1;
	}

	window.LockMouseToWindow(true);
	window.ShowOSPointer(false);

	while(window.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		renderer.UpdateScene(window.GetTimer()->GetTimedMS());
		renderer.RenderScene();

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_E))
		{
			renderer.ToggleCamera();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
		{
			renderer.ToggleSunRotation();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
		{
			renderer.ToggleEdgeDetection();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
		{
			renderer.ToggleShadowMap();
		}
	}

	return 0;
}