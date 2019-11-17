#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	island = new Island();
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));
	sun = new Light(Vector3(HEIGHT * HEIGHTMAP_X / 2.0f, 2000.0f, WIDTH * HEIGHTMAP_Z / 2.0f), Vector4(1, 1, 1, 1), 10000.0f);

	currentShader = new Shader(SHADERDIR"IslandVertex.glsl", SHADERDIR"IslandFragment.glsl");

	if (!currentShader->LinkProgram())
	{
		return;
	}

	island->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetTexture())
	{
		return;
	}

	island->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetBumpMap())
	{
		return;
	}

	island->SetHeightTexture(SOIL_load_OGL_texture(TEXTUREDIR"Bornholm.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetHeightTexture())
	{
		return;
	}

	SetTextureRepeating(island->GetTexture(), true);
	SetTextureRepeating(island->GetBumpMap(), true);

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
	delete sun;
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

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "heightTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	UpdateShaderMatrices();
	SetShaderLight(*sun);

	island->Draw();

	glUseProgram(0);
	SwapBuffers();
}
