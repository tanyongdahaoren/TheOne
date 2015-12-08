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
	_texture->LoadDepthTexture(width, height);

	//-------------------------
	//ATTACHMENT type :
	//1.GL_COLOR_ATTACHMENTi
	//2.GL_DEPTH_ATTACHMENT
	//3.GL_STENCIL_ATTACHMENT
	//4.GL_DEPTH_STENCIL_ATTACHMENT
	//-------------------------

	if (pixelFormat == PixelFormat::depth)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texture->_textureID, 0);
		
		// No color output in the bound framebuffer, only depth.
		glDrawBuffer(GL_NONE);
	}
	else
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture->_textureID, 0);
	}

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error, status: 0x%x\n", Status);
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

void RenderToTexture::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void RenderToTexture::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
