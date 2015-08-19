#include "Texture2D.h"
#include "EasyImage.h"

bool Texture2D::LoadWithImage(EasyImage* easyImage)
{
	const Magick::Image image = easyImage->GetImage();
	const Magick::Blob& blob = easyImage->GetBlob();

	return Load(GL_RGBA, image.columns(), image.rows(), (unsigned char*)blob.data());
}

void Texture2D::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

bool Texture2D::Load(GLenum format, int w, int h, unsigned char * data)
{
	_width = w;
	_height = h;

	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// clean possible GL error
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("glError: 0x%04X", err);
	}

	glTexImage2D(GL_TEXTURE_2D,
		0,
		format,
		_width,
		_height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("glError: 0x%04X", err);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
