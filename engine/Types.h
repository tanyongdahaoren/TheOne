#pragma once

#include "MathH.h"
#include "GLHeader.h"

struct Size
{
	Size();
	Size(float _w, float _h);

	float w;
	float h;
};

struct Size3D
{
	Size3D();
	Size3D(float _w, float _h, float _d);

	float w;
	float h;
	float d;
};

struct Rect
{
	Rect(float x, float y, float width, float height);
	float getMinX() const;
	float getMidX() const;
	float getMaxX() const;
	float getMinY() const;
	float getMidY() const;
	float getMaxY() const; /// return the topmost y-value of current rect
	vec2 origin;
	Size size;
};

struct Color3B
{
	Color3B();
	Color3B(GLubyte _r, GLubyte _g, GLubyte _b);
	
	GLubyte r;
	GLubyte g;
	GLubyte b;

	static const Color3B BLACK;
	static const Color3B WHITE;
	static const Color3B RED;
	static const Color3B GREEN;
	static const Color3B BLUE;
	static const Color3B YELLOW;
};

struct Color3F
{
	Color3F();
	Color3F(GLfloat _r, GLfloat _g, GLfloat _b);

	GLfloat r;
	GLfloat g;
	GLfloat b;

	static const Color3F BLACK;
	static const Color3F WHITE;
	static const Color3F RED;
	static const Color3F GREEN;
	static const Color3F BLUE;
	static const Color3F YELLOW;
};

struct V3F_C3B
{
	vec3 vertex;
	Color3B color;
};

struct V2F_T2F
{
	V2F_T2F() {}

	V2F_T2F(vec2 pos, vec2 tex)
	{
		vertex = pos;
		texcood = tex;
	}
	vec2 vertex;
	vec2 texcood;
};
