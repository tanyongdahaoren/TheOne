#pragma once

#include <Importer.hpp> // C++ importer interface
#include <scene.h>      // Output data structure
#include <postprocess.h> // Post processing flags

#include <map>
#include <vector>
using namespace std;

#include "Types.h"
#include "Ref.h"

#include "Texture2D.h"
#include "Vector.h"
#define INVALID_MATERIAL 0xFFFFFFFF

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
	friend class Sprite2D;
	friend class Sprite3D;
	friend class MeshManager;

	struct MeshEntry 
	{
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}
		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};
public:
	Mesh();
	~Mesh();
	bool InitFromFile(const string& fileName, unsigned int flag);
	void GenBuffers();
	void GenTextures();
	void UseBuffers();
	bool HaveAttribute(int attrib);
protected:
	void InitMaterials(const aiScene* pScene, const std::string& Filename);
	void BindBufferDatas();
	void CalcNormals();
	void CalcTangents();
	float* GetVertex(int attrib, int vertexIdx);
	void SetVertex(int attrib, int vertexIdx, float* pValue);
	void FillVertexAttributeWithFlag();
public:
	unsigned int attribFlag;

	int sizePerVertex;
	int stridePerVertex;

	vector<MeshEntry> _entries;

	//key是attribute
	map<int, MeshVertexAttrib> attribs;

	//每个属性是一个vector key同样是attribute
	map<int, vector<float> > vertexDatas;

	//顶点buffer
	vector<GLuint> indices;

	GLuint _ebo;
	GLuint _vao;
	map<int, GLuint> _vbos;

	bool _bufferDirty;

	vector<string> _textureNames;
	Vector<Texture2D*> _textures;
};


