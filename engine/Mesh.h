#pragma once

#include <vector>
using namespace std;

#include "Types.h"
#include "Ref.h"

class Mesh : public Ref
{
public:
	Mesh();
	~Mesh();

	void GenBuffers();
	void UseBuffers();
	void CalcNormals();
public:
	std::vector<V3F_T2F_V3N> vertices;
	std::vector<GLuint> indices;
	string _textureName;

	GLuint _ebo;
	GLuint _vbo;
	GLuint _vao;
};


