#pragma once

#include "Node.h"
#include "Types.h"
#include "Texture2D.h"
#include "Shader.h"

class Sprite2D : public Node
{
public:
	Sprite2D();
	~Sprite2D();

	void InitWithTexture2D(Texture2D* texture2d);
	void InitBuffers();
protected:
	virtual void Draw(Camera* camera);
protected:
	Size _size;
	Texture2D* _texture2D;

	Shader* _program;

	GLuint _ebo;
	GLuint _vbo;
	GLuint _vao;
};