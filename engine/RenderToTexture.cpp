#include "RenderToTexture.h"

RenderToTexture::RenderToTexture()
	: _fbo(0)
	, _depthRenderBuffer(0)
	, _texture(NULL)
{

}

RenderToTexture::~RenderToTexture()
{
	if (_fbo)
	{
		glDeleteFramebuffers(1, &_fbo);
	}
	if (_depthRenderBuffer)
	{
		glDeleteRenderbuffers(1, &_depthRenderBuffer);
	}
	SAFE_RELEASE(_texture);
}

void RenderToTexture::InitBuffer(int width, int height, PixelFormat pixelFormat)
{
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	_texture = new Texture2D;
	_texture->Retain();
	_texture->Load(pixelFormat, width, height, 0);

	//-------------------------
	//ATTACHMENT type :
	//1.GL_COLOR_ATTACHMENTi
	//2.GL_DEPTH_ATTACHMENT
	//3.GL_STENCIL_ATTACHMENT
	//4.GL_DEPTH_STENCIL_ATTACHMENT
	//-------------------------

	if (pixelFormat != PixelFormat::depth)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture->_textureID, 0);
	}
	else
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texture->_textureID, 0);
	}
}

void RenderToTexture::EnableDepth()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
}
