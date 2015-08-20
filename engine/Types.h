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

struct V3F_T2F_V3N
{
	vec3 vertex;
	vec2 texcood;
	vec3 normal;

	V3F_T2F_V3N(){}

	V3F_T2F_V3N(const vec3& pos, const vec2& tex, const vec3& normal = vec3(0,0,0))
	{
		this->vertex = pos;
		this->texcood = tex;
		this->normal = normal;
	}
};

struct V3F_T2F_V3N_V3T
{
	vec3 vertex;
	vec2 texcood;
	vec3 normal;
	vec3 tangent;

	V3F_T2F_V3N_V3T(){}

	V3F_T2F_V3N_V3T(const vec3& pos, const vec2& tex, const vec3& normal, const vec3& tangent)
	{
		this->vertex = pos;
		this->texcood = tex;
		this->normal = normal;
		this->tangent = tangent;
	}
};
