#pragma once

#include <vector>
using namespace std;

#include "Types.h"
#include "Ref.h"

struct MeshVertexAttrib
{
	MeshVertexAttrib(GLuint size, int vertexAttrib)
	{
		this->size = size;
		this->vertexAttrib = vertexAttrib;
		this->attribSizeBytes = size * sizeof(float);
	}
	//attribute size
	GLint size;
	//GL_FLOAT
	//GLenum type;
	//VERTEX_ATTRIB_POSITION,VERTEX_ATTRIB_COLOR,VERTEX_ATTRIB_TEX_COORD,VERTEX_ATTRIB_NORMAL, VERTEX_ATTRIB_BLEND_WEIGHT, VERTEX_ATTRIB_BLEND_INDEX, GLProgram for detail
	int  vertexAttrib;
	//size in bytes
	int attribSizeBytes;
};

struct MeshVertex
{
	vector<float> oneVertex;
};

class Mesh : public Ref
{
public:
	Mesh();
	~Mesh();

	void GenBuffers();
	void UseBuffers();
	void CalcNormals();
public:
	int sizePerVertex;
	vector<MeshVertexAttrib> attribs;
	std::vector<MeshVertex> vertices;
	std::vector<GLuint> indices;
	string _textureName;

	GLuint _ebo;
	GLuint _vbo;
	GLuint _vao;
};


