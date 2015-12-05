#pragma once

#include "Node.h"
#include "MathH.h"
#include "GLHeader.h"
#include "Types.h"
#include "Shader.h"

class DrawNode : public Node
{
public:
	DrawNode();
	~DrawNode();
	void AddVert(vec3 pos, Color3B c);
	void Clear();
protected:
	virtual void InitBuffers();
	void BindBufferDatas();

	virtual void EnsureCapacity(int count);
	virtual void Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform);
	void SetMode(GLenum mode){ _mode = mode; }
protected:
	Shader* _program;

	GLuint _vbo;
	GLuint _vao;

	V3F_C3B* _buffer;
	int      _vertBufferCount;
	GLsizei  _vertCount;
	bool     _dirty;
public:
	GLenum _mode;
};

class DrawLines : public DrawNode
{
public:
	DrawLines();
	
	void DrawLine(vec3 from, vec3 to, Color3B c1, Color3B c2);

	void SetLineWidth(int w);
protected:
	void Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform);
protected:
	int _lineWidth;
};

class DrawPoints : public DrawNode
{
public:
	DrawPoints();

	void DrawPoint(vec3 pos, Color3B c);

	void SetPointSize(int s);
protected:
	void Render(const mat4& cameraProjTransform, const mat4& cameraViewTransform);
protected:
	int _pointSize;
};

class DrawTriangles : public DrawNode
{
public:
	DrawTriangles();

	void DrawTriangle(vec3 v1, vec3 v2, vec3 v3, Color3B c1, Color3B c2, Color3B c3);
};