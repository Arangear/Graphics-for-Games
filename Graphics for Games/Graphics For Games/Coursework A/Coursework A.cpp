#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main()
{
	Window window("Coursework A", 1920, 1080, true);
	if(!window.HasInitialised())
	{
		return -1;
	}
	
	Renderer renderer(window);
	if(!renderer.HasInitialised())
	{
		return -1;
	}

	window.LockMouseToWindow(true);
	window.ShowOSPointer(false);

	while(window.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		renderer.UpdateScene(window.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}