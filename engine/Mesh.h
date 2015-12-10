#pragma once

#include <Importer.hpp> // C++ importer interface
#include <scene.h>      // Output data structure
#include <postprocess.h> // Post processing flags

#include <map>
#include <vector>
using namespace std;

#include "Types.h"
#include "Ref.h"

#include "Texture.h"
#include "Vector.h"

#define INVALID_MATERIAL 0xFFFFFFFF
#define NUM_BONES_PER_VEREX 4

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

enum MeshAttribStep
{
	MeshAttribStep_pos = 1,
	MeshAttribStep_texcood = 1<<1,
	MeshAttribStep_gen_normal = 1<<2,
	MeshAttribStep_gen_normal_smooth = 1<<3,
	MeshAttribStep_bone = 1<<4,
	MeshAttribStep_tangent = 1<<5,
};


class Mesh : public Ref
{
	friend class Sprite2D;
	friend class Sprite3D;
	friend class SkyBox;
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

	struct BoneInfo
	{
		mat4 BoneOffset;
		mat4 FinalTransformation;
	};
public:
	Mesh();
	~Mesh();
	bool InitFromFile(const string& fileName, uint flag);
	void GenBuffers();
	void GenTextures();
	void UseBuffers();
	bool HaveAttribute(int attrib);
	void BoneTransform(float timeInSeconds);
	void SetNormalTexture(Texture2D* texture2D);
	bool HaveNormalMap();
	bool HaveBone();
	void SetColorTexture(Texture* texture);
	Texture* GetColorTexture();
protected:
	void InitMaterials(const aiScene* pScene, const std::string& Filename);
	void BindBufferDatas();
	void CalcNormals();
	void CalcTangents();
	float* GetVertex(int attrib, int vertexIdx);
	void SetVertex(int attrib, int vertexIdx, float* pValue);
	void FillVertexAttributeWithFlag();
	void LoadBone(const aiMesh* paiMesh, uint idx);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const mat4& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string nodeName);
	uint FindPosition(float animationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float animationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float animationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
public:
	Assimp::Importer _importer;
	const aiScene* _scene;

	unsigned int _attribFlag;

	int _sizePerVertex;
	int _stridePerVertex;

	vector<MeshEntry> _entries;

	//key��attribute
	map<int, MeshVertexAttrib> _attribs;

	//ÿ��������һ��vector keyͬ����attribute
	map<int, vector<float> > _vertexDatas;

	//����buffer
	vector<GLuint> _indices;

	//for buffers
	GLuint _ebo;
	GLuint _vao;
	map<int, GLuint> _vbos;
	bool _bufferDirty;

	//for textures
	vector<string> _textureNames;
	Vector<Texture2D*> _textures;
	Texture2D* _normalTexture;
	Texture*   _colorTexture;

	uint _boneNum;
	map<string, uint> _boneMapping; // maps a bone name to its index
	vector<BoneInfo> _bonesInfo;
	mat4 _globalInverseTransform;
};


