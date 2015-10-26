#include "Mesh.h"
#include "ShaderValue.h"

Mesh::Mesh()
	: _vao(INVALID_OGL_VALUE)
	, _ebo(INVALID_OGL_VALUE)
	, attribFlag(0)
	, sizePerVertex(0)
	, stridePerVertex(0)
{

}

Mesh::~Mesh()
{
	if (_ebo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_ebo);
	if (_vao != INVALID_OGL_VALUE)
		glDeleteVertexArrays(1, &_vao);
	for (auto it:_vbos)
	{
		glDeleteVertexArrays(1, &it.second);
	}
}

bool Mesh::InitFromFile(const string& fileName, unsigned int flag)
{
	this->attribFlag = flag;

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(fileName.c_str(), flag);

	if (!pScene)
	{
		printf("Error parsing '%s': '%s'\n", fileName.c_str(), importer.GetErrorString());
		return false;
	}
	
	//init entries
	_entries.resize(pScene->mNumMeshes);
	uint NumVertices = 0;
	uint NumIndices = 0;
	for (uint i = 0; i < _entries.size(); i++)
	{
		_entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		_entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		_entries[i].BaseVertex = NumVertices;
		_entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += _entries[i].NumIndices;
	}

	//materials
	InitMaterials(pScene, fileName);

	//attributes
	FillVertexAttributeWithFlag();

	//resize array
	for (auto it : attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		this->vertexDatas[attribType].reserve(attrib.size * NumVertices);
	}
	this->indices.reserve(NumIndices);

	// mesh
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < _entries.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		
		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
		{
			//顶点位置
			const aiVector3D* pPos = &paiMesh->mVertices[i];
			this->vertexDatas[eShaderVertAttribute_pos].push_back(pPos->x);
			this->vertexDatas[eShaderVertAttribute_pos].push_back(pPos->y);
			this->vertexDatas[eShaderVertAttribute_pos].push_back(pPos->z);

			//纹理坐标
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
			this->vertexDatas[eShaderVertAttribute_texcood].push_back(pTexCoord->x);
			this->vertexDatas[eShaderVertAttribute_texcood].push_back(pTexCoord->y);

			//法线
			const aiVector3D* pNormal = &paiMesh->mNormals[i];
			this->vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->x);
			this->vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->y);
			this->vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->z);

			//切线
			if (flag & aiProcess_CalcTangentSpace)
			{
				const aiVector3D* pTangent = &(paiMesh->mTangents[i]);
				this->vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->x);
				this->vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->y);
				this->vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->z);
			}
		}

		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
		{
			const aiFace& face = paiMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			this->indices.push_back(face.mIndices[0]);
			this->indices.push_back(face.mIndices[1]);
			this->indices.push_back(face.mIndices[2]);
		}
	}

	return true;
}

void Mesh::GenBuffers()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	for (auto it:attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		GLuint vbo;
		glGenBuffers(1, &vbo);
		_vbos.insert(make_pair(attribType, vbo));
	}

	BindBufferDatas();

	glGenBuffers( 1, &_ebo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW );

	glBindVertexArray(0);
}

void Mesh::BindBufferDatas()
{
	for (auto it : attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[attribType]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexDatas[attribType].size(), &vertexDatas[attribType][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribType);
		glVertexAttribPointer(attribType, attrib.size, GL_FLOAT, GL_FALSE, 0, 0);
	}

	_bufferDirty = false;
}

void Mesh::UseBuffers()
{
	glBindVertexArray(_vao);

	if (_bufferDirty)
	{
		BindBufferDatas();
	}

	for (uint i = 0; i < _entries.size(); i++)
	{
		const uint MaterialIndex = _entries[i].MaterialIndex;
		
		if (MaterialIndex < _textures.size() && _textures[MaterialIndex])
		{
			_textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, _entries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * _entries[i].BaseIndex), _entries[i].BaseVertex);
	}
	
/*	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);*/

	glBindVertexArray(0);
}

bool Mesh::HaveAttribute(int attrib)
{
	auto it = attribs.find(attrib);
	if (it == attribs.end())
	{
		return false;
	}
	return true;
}

void Mesh::CalcNormals()
{
	vertexDatas[eShaderVertAttribute_normal].resize(vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		float* pPos0 = GetVertex(eShaderVertAttribute_pos, index0);
		float* pPos1 = GetVertex(eShaderVertAttribute_pos, index1);
		float* pPos2 = GetVertex(eShaderVertAttribute_pos, index2);
		vec3 pos0 = vec3(*pPos0, *(pPos0 + 1), *(pPos0 + 2));
		vec3 pos1 = vec3(*pPos1, *(pPos1 + 1), *(pPos1 + 2));
		vec3 pos2 = vec3(*pPos2, *(pPos2 + 1), *(pPos2 + 2));
		vec3 v1 = pos1 - pos0;
		vec3 v2 = pos2 - pos0;
		vec3 normal = normalize(cross(v1, v2));

		float* pNormal0 = GetVertex(eShaderVertAttribute_normal, index0);
		float* pNormal1 = GetVertex(eShaderVertAttribute_normal, index1);
		float* pNormal2 = GetVertex(eShaderVertAttribute_normal, index2);
		vec3 normal0 = vec3(*pNormal0, *(pNormal0 + 1), *(pNormal0 + 2));
		vec3 normal1 = vec3(*pNormal1, *(pNormal1 + 1), *(pNormal1 + 2));
		vec3 normal2 = vec3(*pNormal2, *(pNormal2 + 1), *(pNormal2 + 2));
		normal0 += normal;
		normal1 += normal;
		normal2 += normal;
		SetVertex(eShaderVertAttribute_normal, index0, &normal0[0]);
		SetVertex(eShaderVertAttribute_normal, index1, &normal1[0]);
		SetVertex(eShaderVertAttribute_normal, index2, &normal2[0]);
	}

	vector<float>& normals = vertexDatas[eShaderVertAttribute_normal];
	for (unsigned int i = 0; i < normals.size(); i += 3)
	{
		vec3 normal = normalize(vec3(normals[i], normals[i + 1], normals[i + 2]));
		normals[i] = normal.x;
		normals[i + 1] = normal.y;
		normals[i + 2] = normal.z;
	}
}

void Mesh::CalcTangents()
{
	vertexDatas[eShaderVertAttribute_tangent].resize(vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		//pos
		float* pPos0 = GetVertex(eShaderVertAttribute_pos, index0);
		float* pPos1 = GetVertex(eShaderVertAttribute_pos, index1);
		float* pPos2 = GetVertex(eShaderVertAttribute_pos, index2);
		vec3 pos0 = vec3(*pPos0, *(pPos0 + 1), *(pPos0 + 2));
		vec3 pos1 = vec3(*pPos1, *(pPos1 + 1), *(pPos1 + 2));
		vec3 pos2 = vec3(*pPos2, *(pPos2 + 1), *(pPos2 + 2));

		//uv
		float* pUV0 = GetVertex(eShaderVertAttribute_texcood, index0);
		float* pUV1 = GetVertex(eShaderVertAttribute_texcood, index1);
		float* pUV2 = GetVertex(eShaderVertAttribute_texcood, index2);
		vec2 uv0 = vec2(*pUV0, *(pUV0 + 1));
		vec2 uv1 = vec2(*pUV1, *(pUV1 + 1));
		vec2 uv2 = vec2(*pUV2, *(pUV2 + 1));
		
		// Edges of the triangle : postion delta
		vec3 deltaPos1 = pos1 - pos0;
		vec3 deltaPos2 = pos2 - pos0;

		// UV delta
		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;

		float* pTangent0 = GetVertex(eShaderVertAttribute_tangent, index0);
		float* pTangent1 = GetVertex(eShaderVertAttribute_tangent, index1);
		float* pTangent2 = GetVertex(eShaderVertAttribute_tangent, index2);
		vec3 tangent0 = vec3(*pTangent0, *(pTangent0 + 1), *(pTangent0 + 2));
		vec3 tangent1 = vec3(*pTangent1, *(pTangent1 + 1), *(pTangent1 + 2));
		vec3 tangent2 = vec3(*pTangent2, *(pTangent2 + 1), *(pTangent2 + 2));
		tangent0 += tangent;
		tangent1 += tangent;
		tangent2 += tangent;
		SetVertex(eShaderVertAttribute_tangent, index0, &tangent0[0]);
		SetVertex(eShaderVertAttribute_tangent, index1, &tangent1[0]);
		SetVertex(eShaderVertAttribute_tangent, index2, &tangent2[0]);
	}
}

float* Mesh::GetVertex(int attrib, int vertexIdx)
{
	int& size = attribs[attrib].size;
	return &vertexDatas[attrib][vertexIdx * size];
}

void Mesh::SetVertex(int attrib, int vertexIdx, float* pValue)
{
	float* p = GetVertex(attrib, vertexIdx);
	int& attribSize = attribs[attrib].size;
	for (int i = 0; i < attribSize; i++)
	{
		*(p + i) = *(pValue + i);
	}
}

void Mesh::FillVertexAttributeWithFlag()
{
	vector<float> tempV;
	sizePerVertex = 0;
	stridePerVertex = 0;

	attribs.insert(make_pair(eShaderVertAttribute_pos, MeshVertexAttrib(3, eShaderVertAttribute_pos, sizePerVertex)));
	sizePerVertex += 3;
	vertexDatas.insert(make_pair(eShaderVertAttribute_pos, tempV));

	attribs.insert(make_pair(eShaderVertAttribute_texcood, MeshVertexAttrib(2, eShaderVertAttribute_texcood, sizePerVertex)));
	sizePerVertex += 2;
	vertexDatas.insert(make_pair(eShaderVertAttribute_texcood, tempV));

	attribs.insert(make_pair(eShaderVertAttribute_normal, MeshVertexAttrib(3, eShaderVertAttribute_normal, sizePerVertex)));
	sizePerVertex += 3;
	vertexDatas.insert(make_pair(eShaderVertAttribute_normal, tempV));

	if (attribFlag & aiProcess_CalcTangentSpace)
	{
		attribs.insert(make_pair(eShaderVertAttribute_tangent, MeshVertexAttrib(3, eShaderVertAttribute_tangent, sizePerVertex)));
		sizePerVertex += 3;
		vertexDatas.insert(make_pair(eShaderVertAttribute_tangent, tempV));
	}

	for (auto it : attribs)
	{
		stridePerVertex += it.second.attribSizeBytes;
	}
}

void Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) 
	{
		Dir = ".";
	}
	else if (SlashIndex == 0)
	{
		Dir = "/";
	}
	else
	{
		Dir = Filename.substr(0, SlashIndex);
	}

	// Initialize the materials
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Dir + "/" + Path.data;
				_textureNames.push_back(FullPath);
			}
		}
	}
}

void Mesh::GenTextures()
{
	for (int i = 0; i < _textureNames.size(); i++)
	{
		EasyImage* image = new EasyImage;
		image->InitWithFileName(_textureNames[i]);
		Texture2D* texture = new Texture2D;
		texture->LoadWithImage(image);
		_textures.push_back(texture);
	}
}
