//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#include "SceneNode.h"

SceneNode::SceneNode(Mesh * mesh, Vector4 colour) 
{
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texRotate = NULL;
	shader = NULL;
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
	for (unsigned int i = 0; i < uniforms.size(); i++)
	{
		delete uniforms[i];
	}
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw(const OGLRenderer& r) 
{
	if (mesh) 
	{
		mesh->Draw();
	}
}

Matrix4 SceneNode::GetTextureMatrix()
{
	if (rotate)
	{
		textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(*texRotate, Vector3(0.0f, 0.0f, 1.0f));
	}
	return textureMatrix;
}

void SceneNode::BuildUniforms()
{
	for (int i = 0; i < uniforms.size(); i++)
	{
		switch (uniforms[i]->GetType())
		{
		case uniform1i:
			glUniform1i(glGetUniformLocation(shader->GetProgram(), uniforms[i]->GetName()), *(int*)uniforms[i]->GetValue());
			break;
		case uniform3fv:
		{
			glUniform3fv(glGetUniformLocation(shader->GetProgram(), uniforms[i]->GetName()), 1, (float*)uniforms[i]->GetValue());
			Vector3 vec = *(Vector3*)uniforms[i]->GetValue();
			if (uniforms[i]->GetName() == "cameraPos")
			{
				bool a = true;
			}
			break;
		}
		case uniform1fv:
			glUniform1fv(glGetUniformLocation(shader->GetProgram(), uniforms[i]->GetName()), 1, (float*)uniforms[i]->GetValue());
			break;
		}
	}
}

void SceneNode::BindTextures()
{
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(textures[i].GetLocation());
		glBindTexture(textures[i].GetType(), textures[i].GetTexture());
	}
}

void SceneNode::Update(float msec) 
{
	if (parent)
	{
		worldTransform = parent->worldTransform * transform;
	}
	else 
	{
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) 
	{
		(*i)->Update(msec);
	}
}