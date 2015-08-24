#include "Sprite2D.h"
#include "ShaderManager.h"
#include "ShaderValue.h"

Sprite2D::Sprite2D()
	: _texture2D(NULL)
	, _vao(INVALID_OGL_VALUE)
	, _vbo(INVALID_OGL_VALUE)
	, _ebo(INVALID_OGL_VALUE)
	, _program(NULL)
{

}

Sprite2D::~Sprite2D()
{
	if (_vbo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_vbo);
	if (_ebo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_ebo);
	if (_vao != INVALID_OGL_VALUE)
		glDeleteVertexArrays(1, &_vao);
}

void Sprite2D::InitWithTexture2D(Texture2D* texture2d)
{
	_texture2D = texture2d;
	_size = Size(texture2d->_width, texture2d->_height);

	InitBuffers();
}

void Sprite2D::InitBuffers()
{
	_program = ShaderManager::GetInstance()->GetShader(shader_position_texure_2D);

	//------------
	//   v3------v2
	//    |      |
	//   v0------v1
	//---------------
	static GLubyte vertex_idx[] = {
		0, 1, 3,
		1, 2, 3
	};
	V2F_T2F v0 = { vec2(-_size.w/2, -_size.h/2), vec2(0,1) };
	V2F_T2F v1 = { vec2(_size.w/2, -_size.h/2), vec2(1, 1) };
	V2F_T2F v2 = { vec2(_size.w/2, _size.h/2), vec2(1, 0) };
	V2F_T2F v3 = { vec2(-_size.w/2, _size.h/2), vec2(0, 0) };
	V2F_T2F vertex[] = {
		v0,v1,v2,v3
	};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glVertexAttribPointer(eShaderVertAttribute_pos, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_T2F), (GLvoid *)offsetof(V2F_T2F, vertex));
	
	glEnableVertexAttribArray(eShaderVertAttribute_texcood);
	glVertexAttribPointer(eShaderVertAttribute_texcood, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_T2F), (GLvoid *)offsetof(V2F_T2F, texcood));

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_idx), vertex_idx, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_program->Active();
	_program->SetUniformLocationWith1i(UNIFORM_TEXTURE_COLOR_SAMPLER, 0);
}

void Sprite2D::Draw(Camera* camera)
{
	glDisable(GL_CULL_FACE);

	// Use shader
	_program->Active();
	
	const mat4& projectTransform = camera->GetProjectTransform();
	const mat4& viewTransform = camera->GetViewTransform();
	glm::mat4 MVP = projectTransform * viewTransform * _toWorldTransform;
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_MVP, &MVP[0][0]);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glEnableVertexAttribArray(eShaderVertAttribute_texcood);
	
	_texture2D->Bind(GL_TEXTURE0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

	glDisableVertexAttribArray(eShaderVertAttribute_pos);
	glDisableVertexAttribArray(eShaderVertAttribute_texcood);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

