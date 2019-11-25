//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751

#include "Renderer.h"
#include <string>

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	//Initialise scene objects
	island = new Island();
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));

	light = new Light(Vector3(300.0f, 12000.0f, 0), Vector3(0,0,0), Matrix4::Perspective(7900.0f, 16100.0f, 1, 89.0f), Vector4(1, 1, 1, 1), 21000.0f);

	//Set up projection matrix
	cameraProjectionMatrix = Matrix4::Perspective(cameraNear, cameraFar, (float)width / (float)height, 45.0f);
	
	//Load meshes and shaders
	quad = Mesh::GenerateQuad();
	sobelQuad = Mesh::GenerateQuad();
	stone = new OBJMesh();
	stone->LoadOBJMesh(TEXTUREDIR"stone2.obj");
	islandShader = new Shader(SHADERDIR"IslandVertex.glsl", SHADERDIR"IslandFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");
	reflectShader = new Shader(SHADERDIR"ReflectVertex.glsl", SHADERDIR"ReflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	textShader = new Shader(SHADERDIR"TextVertex.glsl", SHADERDIR"TextFragment.glsl");
	sobelShader = new Shader(SHADERDIR"SobelVertex.glsl", SHADERDIR"SobelFragment.glsl");
	sceneShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

	//Load font
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	//Make sure shaders are alright
	if (!lightShader->LinkProgram() ||
		!reflectShader->LinkProgram() ||
		!skyboxShader->LinkProgram() ||
		!textShader->LinkProgram() ||
		!islandShader->LinkProgram() ||
		!sobelShader->LinkProgram() ||
		!sceneShader->LinkProgram())
	{
		return;
	}
	
	//Set textures for all objects
	stone->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"RockSmooth0076_4_L.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	stone->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"rockBump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	island->SetSandTexture(SOIL_load_OGL_texture(TEXTUREDIR"sand.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	island->SetRockTexture(SOIL_load_OGL_texture(TEXTUREDIR"rock.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	island->SetDirtTexture(SOIL_load_OGL_texture(TEXTUREDIR"dirt.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	island->SetHeightTexture(SOIL_load_OGL_texture(TEXTUREDIR"Bornholm.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"bluecloud_ft.jpg", TEXTUREDIR"bluecloud_bk.jpg",
									TEXTUREDIR"bluecloud_up.jpg", TEXTUREDIR"bluecloud_dn.jpg",
									TEXTUREDIR"bluecloud_rt.jpg", TEXTUREDIR"bluecloud_lf.jpg",
									SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	//Make sure it actually worked
	if (!stone->GetTexture() ||
		!stone->GetBumpMap() ||
		!island->GetSandTexture() ||
		!island->GetRockTexture() ||
		!island->GetDirtTexture() ||
		!island->GetHeightTexture() ||
		!quad->GetTexture() ||
		!cubeMap)
	{
		return;
	}

	//Prepare shadow texture and FBO
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	
	glGenFramebuffers(1, &shadowFBO);
	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

	//Prepare scene depth texture
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//Prepare colour textures
	for (int i = 0; i < 2; i++)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &sobelFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	
	//Make sure it worked
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Initialise scene graph
	root = new SceneNode();
	SceneNode* s = new SceneNode();
	//Initisalise Island node
	{
		modelMatrix.ToIdentity();
		textureMatrix.ToIdentity();

		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
		s->SetModelScale(Vector3(1, 1, 1));
		s->SetBoundingRadius(WIDTH * HEIGHTMAP_X * 0.75f);
		s->SetMesh(island);
		s->SetModelMatrix(modelMatrix);
		s->SetTextureMatrix(textureMatrix);
		s->SetShader(islandShader);
		s->SetTransparency(false);

		s->AddUniform(new Uniform(uniform1fv, "heightMod", (void*)&currentHeightMod));
		s->AddUniform(new Uniform(uniform1i, "sandTex", new int(3)));
		s->AddUniform(new Uniform(uniform1i, "rockTex", new int(4)));
		s->AddUniform(new Uniform(uniform1i, "dirtTex", new int(5)));
		s->AddUniform(new Uniform(uniform1i, "heightTex", new int(2)));
		s->AddUniform(new Uniform(uniform3fv, "cameraPos", (void*)&camera->GetPosition()));
		s->AddUniform(new Uniform(uniform1i, "shadowTex", new int(7)));

		s->AddTexture(Texture(GL_TEXTURE7, GL_TEXTURE_2D, shadowTex));

		root->AddChild(s);
	}
	//Initialise water node
	{
		float heightX = WIDTH * HEIGHTMAP_X / 2.0f;
		float heightY = 500.0f;
		float heightZ = HEIGHT * HEIGHTMAP_Z / 2.0f;
		waterRotate = new float(0.0f);

		modelMatrix =
			Matrix4::Translation(Vector3(0, heightY, 0)) *
			Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
			Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
		textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(*waterRotate, Vector3(0.0f, 0.0f, 1.0f));

		s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		s->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
		s->SetModelScale(Vector3(1, 1, 1));
		s->SetBoundingRadius(WIDTH * HEIGHTMAP_X * 0.75f);
		s->SetMesh(quad);
		s->SetShader(reflectShader);
		s->SetModelMatrix(modelMatrix);
		s->SetTextureMatrix(textureMatrix);
		s->SetTransparency(true);

		s->AddUniform(new Uniform(uniform1i, "diffuseTex", new int(0)));
		s->AddUniform(new Uniform(uniform1i, "cubeTex", new int(6)));
		s->AddUniform(new Uniform(uniform3fv, "cameraPos", (void*)&camera->GetPosition()));
		s->AddUniform(new Uniform(uniform1i, "shadowTex", new int(7)));

		s->AddTexture(Texture(GL_TEXTURE6, GL_TEXTURE_CUBE_MAP, cubeMap));
		s->AddTexture(Texture(GL_TEXTURE7, GL_TEXTURE_2D, shadowTex));

		s->SetRotation(true);
		s->SetRotationPointer(waterRotate);

		root->AddChild(s);
	}
	//Initialise stone nodes
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				s = new SceneNode();

				textureMatrix.ToIdentity();
				modelMatrix = Matrix4::Translation(Vector3((i - 2) * 300, 2500, (j - 2) * 300)) * Matrix4::Scale(Vector3(100, 100, 100));
				s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				s->SetTransform(Matrix4::Translation(Vector3((i - 2) * 300, 2500, (j - 2) * 300)));
				s->SetModelScale(Vector3(100, 100, 100));
				s->SetBoundingRadius(200.0f);
				s->SetMesh(stone);
				s->SetModelMatrix(s->GetTransform() * Matrix4::Scale(s->GetModelScale()));
				s->SetTextureMatrix(textureMatrix);
				s->SetShader(lightShader);
				s->SetTransparency(false);
				s->SetDrawBack(true);

				s->AddUniform(new Uniform(uniform1i, "diffuseTex", new int(0)));
				s->AddUniform(new Uniform(uniform1i, "bumpTex", new int(1)));
				s->AddUniform(new Uniform(uniform3fv, "cameraPos", (void*)&camera->GetPosition()));
				s->AddUniform(new Uniform(uniform1i, "shadowTex", new int(7)));

				s->AddTexture(Texture(GL_TEXTURE7, GL_TEXTURE_2D, shadowTex));

				root->AddChild(s);
			}
		}
	}
	SetTextureRepeating(island->GetTexture(), true);
	SetTextureRepeating(island->GetSandTexture(), true);
	SetTextureRepeating(island->GetRockTexture(), true);
	SetTextureRepeating(island->GetDirtTexture(), true);
	SetTextureRepeating(island->GetBumpMap(), true);
	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(stone->GetTexture(), true);

	projMatrix = cameraProjectionMatrix;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void)
{
	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(2, bufferColourTex);

	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &sobelFBO);

	delete light;

	delete root;
	delete island;
	delete camera;
	delete quad;
	delete stone;

	delete islandShader;
	delete reflectShader;
	delete lightShader;
	delete skyboxShader;
	delete textShader;
	delete sobelShader;
	delete sceneShader;

	delete font;
	delete waterRotate;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec)
{
	//Update camera, matrices and frustum
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	*waterRotate += msec / 1000.0f;
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	//Every second(-ish) update FPS display
	if (msCount >= 1000)
	{
		msCount = 0;
		fps = std::to_string((int)(1000.0f / msec));
	}
	else
	{
		msCount += msec;
	}
	//Simulate height increase
	if (currentHeightMod < 5000.0f)
	{
		currentHeightMod += growthSpeed * msec / 1000.0f;
	}
	//Rotate the point light if toggled
	if (sunRotation)
	{
		light->SetPosition((Matrix4::Rotation(sunSpeed * msec / 1000.0f, Vector3(1, 0, 0)) * Matrix4::Translation(light->GetPosition())).GetPositionVector());
	}

	//Update nodes
	root->Update(msec);
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw shadowmap
	DrawShadowScene(light);
	//Draw scene
	DrawCombinedScene();
	//Apply postprocessing
	if (edgeDetection)
	{
		DrawSobel();
		PresentScene();
	}
	DrawFPS();

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

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - currentCameraPosition;
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->IsTransparent())
		{
			transparentNodes.push_back(from);
		}
		else
		{
			opaqueNodes.push_back(from);
		}
	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodes.begin(), transparentNodes.end(), SceneNode::CompareByCameraDistance);
	std::sort(opaqueNodes.begin(), opaqueNodes.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes(bool shadow)
{
	for (vector<SceneNode*>::const_iterator i = opaqueNodes.begin(); i != opaqueNodes.end(); ++i)
	{
		DrawNode((*i), shadow);
	}

	if (!shadow)
	{
		for (vector<SceneNode*>::const_reverse_iterator i = transparentNodes.rbegin(); i != transparentNodes.rend(); ++i)
		{
			DrawNode((*i), false);
		}
	}
}

void Renderer::DrawNode(SceneNode* node, bool shadow)
{
	if (node->GetMesh())
	{
		if (shadow)
		{
			glDisable(GL_CULL_FACE);
		}

		SetCurrentShader(node->GetShader());
		for(int i=0;i<2;i++)
		{
			SetShaderLight(*light);
		}

		node->BindTextures();
		node->BuildUniforms();

		//Update matrices
		modelMatrix = node->GetModelMatrix();
		textureMatrix = node->GetTextureMatrix();

		UpdateShaderMatrices();

		node->Draw(*this);

		if (shadow)
		{
			glEnable(GL_CULL_FACE);
		}

		glUseProgram(0);
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodes.clear();
	opaqueNodes.clear();
}

void Renderer::DrawShadowScene(const Light* light)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	currentCameraPosition = light->GetPosition();
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), light->GetDirection());
	projMatrix = light->GetProjectionMatrix();
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	
	UpdateShaderMatrices();

	ClearNodeLists();
	BuildNodeLists(root);
	SortNodeLists();

	DrawNodes(true);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	
	projMatrix = cameraProjectionMatrix;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

void Renderer::DrawCombinedScene()
{
	currentCameraPosition = camera->GetPosition();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = cameraProjectionMatrix;
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	ClearNodeLists();
	BuildNodeLists(root);
	SortNodeLists();

	UpdateShaderMatrices();

	if (edgeDetection)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	DrawSkybox();
	DrawNodes();

	if (edgeDetection)
	{
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::DrawSobel()
{
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, sobelFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	SetCurrentShader(sobelShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	
	glDisable(GL_DEPTH_TEST);

	//Apply slight blur
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "stage"), 0);
	
	sobelQuad->SetTexture(bufferColourTex[0]);
	sobelQuad->Draw();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "stage"), 1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	
	sobelQuad->SetTexture(bufferColourTex[1]);
	sobelQuad->Draw();
	//Apply Sobel filter
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "stage"), 2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	
	sobelQuad->SetTexture(bufferColourTex[0]);
	sobelQuad->Draw();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	sobelQuad->SetTexture(bufferColourTex[1]);
	sobelQuad->Draw();
	glUseProgram(0);
}

void Renderer::DrawFPS()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	SetCurrentShader(textShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	float size = 16.0f;
	TextMesh* mesh = new TextMesh(fps + "fps", *font);

	modelMatrix = Matrix4::Translation(Vector3(0, height , 0)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	mesh->Draw();

	projMatrix = cameraProjectionMatrix;
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(*waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	delete mesh;

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(0);
}
