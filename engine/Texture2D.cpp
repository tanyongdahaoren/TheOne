#include "Texture2D.h"
#include "EasyImage.h"

bool Texture2D::LoadWithImage(EasyImage* easyImage)
{
	const Magick::Image image = easyImage->GetImage();
	const Magick::Blob& blob = easyImage->GetBlob();

	_width = image.columns();
	_height = image.rows();

	GLenum err = glGetError();

	glGenTextures(1, &_textureID);

	err = glGetError();

	glBindTexture(GL_TEXTURE_2D, _textureID);

	err = glGetError();

	glTexImage2D(GL_TEXTURE_2D, 
		0, 
		GL_RGBA,
		_width,
		_height,
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE,
		blob.data());
	err = glGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	if (err != GL_NO_ERROR)
	{
		printf("glError: 0x%04X", err);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void Texture2D::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}
