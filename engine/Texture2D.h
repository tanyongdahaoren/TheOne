#pragma once

#include "EasyImage.h"
#include "GLHeader.h"
#include "Types.h"

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