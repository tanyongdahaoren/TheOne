#pragma once

#include "EasyImage.h"
#include "GLHeader.h"
#include "Types.h"

#define COLOR_TEXTURE              GL_TEXTURE0
#define COLOR_TEXTURE_INDEX        0
#define NORMAL_TEXTURE             GL_TEXTURE1
#define NORMAL_TEXTURE_INDEX       1
#define DEPTH_TEXTURE              GL_TEXTURE2
#define DEPTH_TEXTURE_INDEX        2
#define SHADOW_MAP_TEXTURE_DIRECTION_LIGHT				GL_TEXTURE10
#define SHADOW_MAP_TEXTURE_DIRECTION_LIGHT_INDEX        10
enum eFilterType
{
	eFilterType_nearest = GL_NEAREST,
	eFilterType_linear = GL_LINEAR,
};

enum eWrapType
{
	eWrapType_reapeat = GL_REPEAT,
	eWrapType_edge = GL_CLAMP_TO_EDGE,
	eWrapType_mirrored_repeat = GL_MIRRORED_REPEAT,
};

enum class PixelFormat
{
	AUTO,
	
	RGBA8888,
	RGB888,
	RGB565,
	depth,
};

struct PixelFormatInfo
{
	PixelFormatInfo(GLenum anInternalFormat, GLenum aFormat, GLenum aType, int aBpp, bool aCompressed, bool anAlpha)
		: internalFormat(anInternalFormat)
		, format(aFormat)
		, type(aType)
		, bpp(aBpp)
		, compressed(aCompressed)
		, alpha(anAlpha)
	{}

	GLenum internalFormat;
	GLenum format;
	GLenum type;
	int bpp;
	bool compressed;
	bool alpha;
};

typedef std::map<PixelFormat, PixelFormatInfo> PixelFormatInfoMap;

class Texture2D : public Ref
{
public:
	bool LoadWithImage(EasyImage* image);
	bool Load(PixelFormat pixelFormat, int w, int h, unsigned char * data);
	void Bind(GLenum TextureUnit);
	void SetFilterType(eFilterType type);
	void SetWrapType(eWrapType type);
public:
	GLuint _textureID;
	unsigned short _width;
	unsigned short _height;

	static const PixelFormatInfoMap _pixelFormatInfoTables;
};