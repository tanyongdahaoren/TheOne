#pragma once

#include "GLHeader.h"
#include "Texture.h"

class RenderToTexture
{
public:
	RenderToTexture();
	~RenderToTexture();
	void InitBuffer(int width, int height, PixelFormat pixelFormat);
	void EnableDepth();
	void Bind();
	void UnBind();
public:
	GLuint _fbo;
	GLuint _depthRenderBuffer;
	Texture2D* _texture;
};