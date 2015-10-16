#include "DrawNode.h"
#include "ShaderManager.h"
#include "ShaderValue.h"
#include "Camera.h"

//---------------
//DrawNode
//---------------
void DrawNode::InitBuffers()
{
	EnsureCapacity(32);

	_dirty = true;

	_program = ShaderManager::GetInstance()->GetShader(shader_position_color);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C3B)*_vertBufferCount, _buffer, GL_STREAM_DRAW);

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glEnableVertexAttribArray(eShaderVertAttribute_color);

	glVertexAttribPointer(eShaderVertAttribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C3B), (GLvoid *)offsetof(V3F_C3B, vertex));
	glVertexAttribPointer(eShaderVertAttribute_color, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C3B), (GLvoid *)offsetof(V3F_C3B, color));

	glDisableVertexAttribArray(eShaderVertAttribute_pos);
	glDisableVertexAttribArray(eShaderVertAttribute_color);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void DrawNode::EnsureCapacity(int count)
{
	if (_vertCount + count > _vertBufferCount)
	{
		_vertBufferCount += max(_vertBufferCount, count);
		_buffer = (V3F_C3B*)realloc(_buffer, _vertBufferCount*sizeof(V3F_C3B));
	}
}

DrawNode::DrawNode()
	: _vbo(0)
	, _vao(0)
	, _buffer(NULL)
	, _vertBufferCount(0)
	, _vertCount(0)
	, _dirty(false)
{
	InitBuffers();
}

DrawNode::~DrawNode()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	_vao = _vbo = 0;
}

void DrawNode::Render(Camera* camera)
{
	// Use our shader
	_program->Active();

	const mat4& projectTransform = camera->GetProjectTransform();
	const mat4& viewTransform = camera->GetViewTransform();
	glm::mat4 MVP = projectTransform * viewTransform * _toWorldTransform;
	_program->SetUniformLocationWithMatrix4fv(UNIFORM_MVP, &MVP[0][0]);

	if (_dirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C3B)*_vertBufferCount, _buffer, GL_STREAM_DRAW);
		_dirty = false;
	}

	glBindVertexArray(_vao);

	glEnableVertexAttribArray(eShaderVertAttribute_pos);
	glEnableVertexAttribArray(eShaderVertAttribute_color);

	glDrawArrays(_mode, 0, _vertCount);

	glDisableVertexAttribArray(eShaderVertAttribute_pos);
	glDisableVertexAttribArray(eShaderVertAttribute_color);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void DrawNode::AddVert(vec3 pos, Color3B c)
{
	EnsureCapacity(1);

	V3F_C3B* point = (V3F_C3B*)(_buffer + _vertCount);

	V3F_C3B newP = { pos, c };

	*point = newP;

	_vertCount ++;
	_dirty = true;
}

void DrawNode::Clear()
{
	_vertBufferCount = 0;
	_vertCount = 0;
	_dirty = true;
}

//---------------
//DrawLines
//---------------
DrawLines::DrawLines()
	: _lineWidth(2)
{
	SetMode(GL_LINES);
}

void DrawLines::DrawLine(vec3 from, vec3 to, Color3B c1, Color3B c2)
{
	AddVert(from, c1);
	AddVert(to, c2);
}

void DrawLines::Render(Camera* camera)
{
	glLineWidth(_lineWidth);

	DrawNode::Render(camera);
}

void DrawLines::SetLineWidth(int w)
{
	_lineWidth = w;
}


//---------------
//DrawPoints
//---------------
DrawPoints::DrawPoints()
	: _pointSize(5)
{
	SetMode(GL_POINTS);
}

void DrawPoints::DrawPoint(vec3 pos, Color3B c)
{
	AddVert(pos, c);
}

void DrawPoints::Render(Camera* camera)
{
	glPointSize(_pointSize);

	DrawNode::Render(camera);
}

void DrawPoints::SetPointSize(int s)
{
	_pointSize = s;
}


//---------------
//DrawTriangles
//---------------
DrawTriangles::DrawTriangles()
{
	SetMode(GL_TRIANGLES);
}

void DrawTriangles::DrawTriangle(vec3 v1, vec3 v2, vec3 v3, Color3B c1, Color3B c2, Color3B c3)
{
	AddVert(v1, c1);
	AddVert(v2, c2);
	AddVert(v3, c3);
}

