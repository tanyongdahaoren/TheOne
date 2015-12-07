#include "Texture2D.h"
#include "EasyImage.h"
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

bool Texture2D::LoadTexture2D(string image)
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
		flag = SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT;
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

bool Texture2D::LoadWithImage(EasyImage* easyImage)
{
	const Magick::Image image = easyImage->GetImage();
	const Magick::Blob& blob = easyImage->GetBlob();

	return Load(PixelFormat::RGBA8888, image.columns(), image.rows(), (unsigned char*)blob.data());
}

bool Texture2D::Load(PixelFormat pixelFormat, int w, int h, unsigned char * data)
{
	const PixelFormatInfo& info = _pixelFormatInfoTables.at(pixelFormat);

	_width = w;
	_height = h;

	// clean possible GL error
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

	if (pixelFormat == PixelFormat::depth)
	{
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, info.internalFormat, _width, _height, 0, info.format, info.type, data);

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
