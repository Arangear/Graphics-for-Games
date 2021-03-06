//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/Island.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/OBJMesh.h"
#include "TextMesh.h"
#include <algorithm>

#define SHADOWSIZE 4096

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void ToggleCamera() { camera->ToggleAutomated(); }
	void ToggleSunRotation() { sunRotation = !sunRotation; }
	void ToggleEdgeDetection() { edgeDetection = !edgeDetection; }
	void ToggleShadowMap() { shadowMapOn = !shadowMapOn; }
	void ToggleSplitScreen() { splitOn = !splitOn; }

protected:
	Frustum frameFrustum;
	SceneNode* root;
	vector<SceneNode*> opaqueNodes;
	vector<SceneNode*> transparentNodes;

	Matrix4 cameraProjectionMatrix;

	OBJMesh* stone;
	Island* island;
	Camera* camera;
	Light* light;
	Mesh* quad;
	Mesh* sobelQuad;
	GLuint cubeMap;
	float* waterRotate;

	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* islandShader;
	Shader* lightShader;
	Shader* textShader;
	Shader* sobelShader;
	Shader* sceneShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferFBO;
	GLuint sobelFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	//Allows correct frustum calculation
	Vector3 currentCameraPosition;

	Font* font;
	TextMesh* text;
	std::string fps = "60";
	int msCount = 0;

	float growthSpeed = 150.0f;
	float currentHeightMod = 0.0f;
	float cameraFar = 30000.0f;
	float cameraNear = 50.0f;

	float sunSpeed = 30.0f;
	bool sunRotation = false;

	bool edgeDetection = false;
	bool shadowMapOn = false;
	bool splitOn = false;

	void DrawSkybox();
	void DrawFPS();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void DrawNodes(bool shadow = false);
	void DrawNode(SceneNode* node, bool shadow = false);
	void ClearNodeLists();
	void DrawShadowScene(const Light* light);
	void DrawCombinedScene();
	void DrawSobel();
	void DrawSkyboxAndNodes();
	void PresentScene();

	int GetObjectCount() { return opaqueNodes.size() + transparentNodes.size(); }
};
