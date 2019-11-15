#pragma once

#include "..\nclgl\mesh.h"

#define WIDTH 1024
#define HEIGHT 1024

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f
#define HEIGHT_TEX_X 1.0f / 1024.0f
#define HEIGHT_TEX_Z 1.0f / 1024.0f

class Island : public Mesh
{
public:
	Island();
	~Island();

	void Draw();

	void SetHeightTexture(GLuint tex) { heightTexture = tex; }
	GLuint GetHeightTexture() { return heightTexture; }

private:
	GLuint heightTexture;
	Vector2* heightCoords;

	void BufferAllData();
};