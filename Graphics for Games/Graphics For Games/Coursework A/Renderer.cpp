#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	island = new Island();
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));
	sun = new Light(Vector3(HEIGHT * HEIGHTMAP_X / 2.0f, 2000.0f, WIDTH * HEIGHTMAP_Z / 2.0f), Vector4(1, 1, 1, 1), 10000.0f);
	quad = Mesh::GenerateQuad();

	lightShader = new Shader(SHADERDIR"IslandVertex.glsl", SHADERDIR"IslandFragment.glsl");
	reflectShader = new Shader(SHADERDIR"ReflectVertex.glsl", SHADERDIR"ReflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");

	if (!lightShader->LinkProgram() || !reflectShader->LinkProgram() || !skyboxShader->LinkProgram())
	{
		return;
	}

	island->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetTexture())
	{
		return;
	}

	island->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetBumpMap())
	{
		return;
	}

	island->SetHeightTexture(SOIL_load_OGL_texture(TEXTUREDIR"Bornholm.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!island->GetHeightTexture())
	{
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!quad->GetTexture())
	{
		return;
	}

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"sgod_lf.tga", TEXTUREDIR"sgod_rt.tga", 
									TEXTUREDIR"sgod_up.tga", TEXTUREDIR"sgod_dn.tga",
									TEXTUREDIR"sgod_bk.tga", TEXTUREDIR"sgod_ft.tga",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!cubeMap)
	{
		return;
	}

	SetTextureRepeating(island->GetTexture(), true);
	SetTextureRepeating(island->GetBumpMap(), true);
	SetTextureRepeating(quad->GetTexture(), true);

	projMatrix = Matrix4::Perspective(1.0f, 30000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
	waterRotate = 0.0f;
}

Renderer::~Renderer(void)
{
	delete island;
	delete camera;
	delete sun;
	delete quad;
	delete reflectShader;
	delete lightShader;
	delete skyboxShader;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSkybox();
	DrawIsland();
	DrawWater();

	SwapBuffers();
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawIsland()
{
	SetCurrentShader(lightShader);
	SetShaderLight(*sun);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "heightTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	island->Draw();

	glUseProgram(0);
}

void Renderer::DrawWater()
{
	SetCurrentShader(reflectShader);
	SetShaderLight(*sun);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = WIDTH * HEIGHTMAP_X / 2.0f;
	float heightY = 500.0f;
	float heightZ = HEIGHT * HEIGHTMAP_Z / 2.0f;

	modelMatrix = 
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}