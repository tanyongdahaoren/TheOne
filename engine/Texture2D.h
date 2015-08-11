#pragma once

#include "EasyImage.h"
#include "GLHeader.h"
#include "Types.h"

class Texture2D : public Ref
{
public:
	bool LoadWithImage(EasyImage* image);
	void Bind(GLenum TextureUnit);
public:
	GLuint _textureID;
	unsigned short _width;
	unsigned short _height;
};