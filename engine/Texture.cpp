#include "Texture.h"
#include "Defines.h"
#include "SOIL.h"
#include "FileUtils.h"

typedef PixelFormatInfoMap::value_type PixelFormatInfoMapValue;
static const PixelFormatInfoMapValue TexturePixelFormatInfoTablesValue[] =
{
	PixelFormatInfoMapValue(PixelFormat::RGBA8888, PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true)),
	PixelFormatInfoMapValue(PixelFormat::RGB565,   PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16, false, false)),
	PixelFormatInfoMapValue(PixelFormat::RGB888,   PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 24, false, false)),
	PixelFormatInfoMapValue(PixelFormat::depth,    PixelFormatInfo(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, 4, false, false)),
};

const PixelFormatInfoMap Texture2D::_pixelFormatInfoTables(TexturePixelFormatInfoTablesValue,
	TexturePixelFormatInfoTablesValue + ARRAY_SIZE(TexturePixelFormatInfoTablesValue));

bool Texture2D::LoadTextureFromImage(string image)
{
	GLuint tex_id = 0;
	string extensionName = FileUtils::GetExtensionName(image.c_str());

	uint flag = 0;

	if (extensionName == "dds")
	{
		flag = SOIL_FLAG_DDS_LOAD_DIRECT;
	}
	else
	{
		flag = SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT;
	}

	GLuint tex_2d = SOIL_load_OGL_texture(image.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flag
		,&_width
		,&_height);
	if (0 == tex_2d)
	{
		return false;
	}
	else
	{
		_textureID = tex_2d;
	}
	return true;
}

bool Texture2D::LoadDepthTexture(int w, int h)
{
	_width = w;
	_height = h;
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("glError: 0x%04X", err);
	}

	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("glError: 0x%04X", err);
		return false;
	}

	return true;
}

void Texture2D::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture2D::SetFilterType(eFilterType type)
{
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
}

void Texture2D::SetWrapType(eWrapType type)
{
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, type);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, type);
}





bool TextureCubeMap::LoadTextureFromImages(const string& PosXFilename, const string& NegXFilename, const string& PosYFilename, const string& NegYFilename, const string& PosZFilename, const string& NegZFilename)
{
	GLuint tex_cube = SOIL_load_OGL_cubemap(
		PosXFilename.c_str(),
		NegXFilename.c_str(),
		PosYFilename.c_str(),
		NegYFilename.c_str(),
		PosZFilename.c_str(),
		NegZFilename.c_str(),
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS);
	if (0 == tex_cube)
	{
		return false;
	}
	else
	{
		_textureID = tex_cube;
	}
	return true;
}

void TextureCubeMap::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
}
