#pragma once

#include "EasyImage.h"
#include "GLHeader.h"
#include "Types.h"

#define COLOR_TEXTURE              GL_TEXTURE0
#define COLOR_TEXTURE_INDEX        0
#define NORMAL_TEXTURE             GL_TEXTURE1
#define NORMAL_TEXTURE_INDEX       1

class Texture2D : public Ref
{
public:
	bool LoadWithImage(EasyImage* image);
	bool Load(GLenum format, int w, int h, unsigned char * data);
	void Bind(GLenum TextureUnit);
public:
	GLuint _textureID;
	unsigned short _width;
	unsigned short _height;
};