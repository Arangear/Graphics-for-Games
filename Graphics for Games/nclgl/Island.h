//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#pragma once

#include "..\nclgl\mesh.h"

#define WIDTH 1024
#define HEIGHT 1024

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 512.0f
#define HEIGHTMAP_TEX_Z 1.0f / 512.0f
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

	void SetSandTexture(GLuint tex) { sandTexture = tex; }
	GLuint GetSandTexture() { return sandTexture; }

	void SetRockTexture(GLuint tex) { rockTexture = tex; }
	GLuint GetRockTexture() { return rockTexture; }

	void SetDirtTexture(GLuint tex) { dirtTexture = tex; }
	GLuint GetDirtTexture() { return dirtTexture; }

private:
	GLuint heightTexture;
	Vector2* heightCoords;

	GLuint sandTexture;
	GLuint dirtTexture;
	GLuint rockTexture;

	void BufferAllData();
};