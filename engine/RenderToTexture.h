#pragma once

#include "GLHeader.h"
#include "Texture2D.h"

class RenderToTexture
{
public:
	RenderToTexture();
	~RenderToTexture();
	void InitBuffer(int width, int height, PixelFormat pixelFormat);
	void EnableDepth();
public:
	GLuint _fbo;
	GLuint _depthRenderBuffer;
	Texture2D* _texture;
};