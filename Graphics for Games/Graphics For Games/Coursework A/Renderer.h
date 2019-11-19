#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Island.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "TextMesh.h"
#include <algorithm>

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	Frustum frameFrustum;
	SceneNode* root;
	vector<SceneNode*> opaqueNodes;
	vector<SceneNode*> transparentNodes;

	Island* island;
	Camera* camera;
	Light* sun;
	Mesh* quad;
	GLuint cubeMap;
	float* waterRotate;

	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* lightShader;
	Shader* textShader;

	Font* font;
	TextMesh* text;
	std::string fps = "60";
	int msCount = 0;

	float growthSpeed = 150.0f;
	float currentHeightMod = 0.0f;
	float cameraFar = 30000.0f;

	void DrawSkybox();
	void DrawFPS();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* node);
	void ClearNodeLists();
};
