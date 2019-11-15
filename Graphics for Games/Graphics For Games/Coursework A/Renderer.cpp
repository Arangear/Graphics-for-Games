#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	island = new Island();
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");

	if (!currentShader->LinkProgram())
	{
		return;
	}

	island->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetTexture())
	{
		return;
	}
	SetTextureRepeating(island->GetTexture(), true);

	island->SetHeightTexture(SOIL_load_OGL_texture(TEXTUREDIR"island5.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetHeightTexture())
	{
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 30000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}

Renderer::~Renderer(void)
{
	delete island;
	delete camera;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "heightTex"), 2);

	island->Draw();

	glUseProgram(0);
	SwapBuffers();
}
