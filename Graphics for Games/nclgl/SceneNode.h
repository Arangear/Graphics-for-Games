#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

enum uniformType { uniform1i, uniform3fv };

class Texture
{
public:
	Texture(GLenum location, GLenum type, GLuint texture) : location(location), type(type), texture(texture) {};

	GLuint GetTexture() { return texture; }
	GLenum GetLocation() { return location; }
	GLenum GetType() { return type; }

private:
	GLenum type;
	GLenum location;
	GLuint texture;
};

class Uniform
{
public:
	Uniform(uniformType type, GLchar* name, void* value) : type(type), name(name), value(value) {};

	uniformType GetType() { return type; }
	GLchar* GetName() { return name; }
	void* GetValue() { return value; }

private:
	uniformType type;
	GLchar* name;
	void* value;
};

class SceneNode 
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);
	
	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	
	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }
	
	void AddChild(SceneNode* s);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}
	
	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer& r);
	
	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() 
	{
		return children.begin();
	}
	
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() 
	{
		return children.end();
	}

	void SetShader(Shader* s) { shader = s; }
	Shader* GetShader() { return shader; }

	void SetModelMatrix(Matrix4 matrix) { modelMatrix = matrix; }
	Matrix4 GetModelMatrix() { return modelMatrix; }

	void SetTextureMatrix(Matrix4 matrix) { textureMatrix = matrix; }
	Matrix4 GetTextureMatrix();

	void AddUniform(Uniform* uniform) { uniforms.push_back(uniform); }
	void AddTexture(Texture tex) { textures.push_back(tex); }

	void SetRotation(bool value) { rotate = value; }

	void SetRotationPointer(float* rotation) { texRotate = rotation; }

	void BuildUniforms();
	void BindTextures();

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;

	Shader* shader;
	Matrix4 modelMatrix;
	Matrix4 textureMatrix;
	std::vector<Uniform*> uniforms;
	std::vector<Texture> textures;

	bool rotate = false;
	float* texRotate;

	std::vector<SceneNode*> children;
	float distanceFromCamera;
	float boundingRadius;
};
