#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Island.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
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
	float waterRotate;

	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* lightShader;

	void DrawSkybox();
	void DrawIsland();
	void DrawWater();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* node);
	void ClearNodeLists();
};
