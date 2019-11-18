#include "SceneNode.h"

SceneNode::SceneNode(Mesh * mesh, Vector4 colour) 
{
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
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
			glUniform3fv(glGetUniformLocation(shader->GetProgram(), uniforms[i]->GetName()), 1, (float*)uniforms[i]->GetValue());
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
	for (vector < SceneNode* >::iterator i = children.begin(); i != children.end(); ++i) 
	{
		(*i)->Update(msec);
	}
}