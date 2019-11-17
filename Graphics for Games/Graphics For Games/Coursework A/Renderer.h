#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Island.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	Island* island;
	Camera* camera;
	Light* sun;
	Mesh* quad;
	GLuint cubeMap;
	float waterRotate;

	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* lightShader;

	void DrawSkybox();
	void DrawIsland();
	void DrawWater();
};
