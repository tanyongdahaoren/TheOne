#pragma once

#include <map>
#include <vector>
using namespace std;

#include "Types.h"
#include "Ref.h"

#define VERTEX_CAL_NORMAL true
#define VERTEX_CAL_TANGENT true

struct MeshVertexAttrib
{
	MeshVertexAttrib(){}
	MeshVertexAttrib(GLuint size, int vertexAttrib, int offset)
	{
		this->size = size;
		this->vertexAttrib = vertexAttrib;
		this->attribSizeBytes = size * sizeof(float);
		this->offset = offset;
	}
	//attribute size
	GLint size;
	//GL_FLOAT
	//GLenum type;
	//VERTEX_ATTRIB_POSITION,VERTEX_ATTRIB_COLOR,VERTEX_ATTRIB_TEX_COORD,VERTEX_ATTRIB_NORMAL, VERTEX_ATTRIB_BLEND_WEIGHT, VERTEX_ATTRIB_BLEND_INDEX, GLProgram for detail
	int  vertexAttrib;
	//size in bytes
	int attribSizeBytes;

	int offset;
};

class Mesh : public Ref
{
	friend class Sprite3D;
	friend class MeshManager;
public:
	Mesh();
	~Mesh();
	void GenBuffers();
	void UseBuffers();
	bool HaveAttribute(int attrib);
protected:
	void CalcNormals();
	void CalcTangents();
	float* GetVertex(int attrib, int vertexIdx);
	void SetVertex(int attrib, int vertexIdx, float* pValue);
public:
	int sizePerVertex;
	int stridePerVertex;

	//key��attribute
	map<int, MeshVertexAttrib> attribs;

	//ÿ��������һ��vector keyͬ����attribute
	map<int, vector<float> > vertexDatas;

	vector<GLuint> indices;
	string _textureName;

	GLuint _ebo;
	GLuint _vao;
	map<int, GLuint> _vbos;
};


