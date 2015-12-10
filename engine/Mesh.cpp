#include "Mesh.h"
#include "ShaderValue.h"

mat4 ConvertAiMatrix4x4ToMat4(aiMatrix4x4 aim)
{
	mat4 m;
	m[0] = vec4(aim.a1, aim.b1, aim.c1, aim.d1);
	m[1] = vec4(aim.a2, aim.b2, aim.c2, aim.d2);
	m[2] = vec4(aim.a3, aim.b3, aim.c3, aim.d3);
	m[3] = vec4(aim.a4, aim.b4, aim.c4, aim.d4);
	return m;
}

mat4 ConvertAiMatrix3x3ToMat4(aiMatrix3x3 aim)
{
	mat4 m;
	m[0] = vec4(aim.a1, aim.b1, aim.c1, 0);
	m[1] = vec4(aim.a2, aim.b2, aim.c2, 0);
	m[2] = vec4(aim.a3, aim.b3, aim.c3, 0);
	m[3] = vec4(0,      0,      0,      1);
	return m;
}

Mesh::Mesh()
	: _vao(INVALID_OGL_VALUE)
	, _ebo(INVALID_OGL_VALUE)
	, _attribFlag(0)
	, _sizePerVertex(0)
	, _stridePerVertex(0)
	, _scene(NULL)
	, _boneNum(0)
	, _normalTexture(NULL)
	, _colorTexture(NULL)
{

}

Mesh::~Mesh()
{
	SAFE_RELEASE(_normalTexture);
	SAFE_RELEASE(_colorTexture);
	if (_ebo != INVALID_OGL_VALUE)
		glDeleteBuffers(1, &_ebo);
	if (_vao != INVALID_OGL_VALUE)
		glDeleteVertexArrays(1, &_vao);
	for (auto it:_vbos)
	{
		glDeleteVertexArrays(1, &it.second);
	}
}

bool Mesh::InitFromFile(const string& fileName, uint flag)
{
	this->_boneNum = 0;
	this->_attribFlag = flag;

	uint loadMeshFlag = 0;
	if (flag & MeshAttribStep_pos)
		loadMeshFlag |= aiProcess_Triangulate;
	if (flag & MeshAttribStep_texcood)
		loadMeshFlag |= aiProcess_FlipUVs;
	if (flag & MeshAttribStep_gen_normal)
		loadMeshFlag |= aiProcess_GenNormals;
	if (flag & MeshAttribStep_gen_normal_smooth)
		loadMeshFlag |= aiProcess_GenSmoothNormals;
	if (flag & MeshAttribStep_tangent)
		loadMeshFlag |= aiProcess_CalcTangentSpace;
	
	_scene = _importer.ReadFile(fileName.c_str(), loadMeshFlag);

	if (!_scene)
	{
		printf("Error parsing '%s': '%s'\n", fileName.c_str(), _importer.GetErrorString());
		return false;
	}
	
	_globalInverseTransform = ConvertAiMatrix4x4ToMat4(_scene->mRootNode->mTransformation);
	_globalInverseTransform = glm::inverse(_globalInverseTransform);

	//materials
	InitMaterials(_scene, fileName);

	//init entries
	_entries.resize(_scene->mNumMeshes);
	uint NumVertices = 0;
	uint NumIndices = 0;
	for (uint i = 0; i < _entries.size(); i++)
	{
		_entries[i].MaterialIndex = _scene->mMeshes[i]->mMaterialIndex;
		_entries[i].NumIndices = _scene->mMeshes[i]->mNumFaces * 3;
		_entries[i].BaseVertex = NumVertices;
		_entries[i].BaseIndex = NumIndices;

		NumVertices += _scene->mMeshes[i]->mNumVertices;
		NumIndices += _entries[i].NumIndices;
	}

	this->_indices.reserve(NumIndices);

	//attributes
	FillVertexAttributeWithFlag();

	//resize array
	for (auto it : _attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		if (attribType == eShaderVertAttribute_blend_index || attribType == eShaderVertAttribute_blend_weight)
		{
			this->_vertexDatas[attribType].resize(attrib.size * NumVertices);
		}
		else
		{
			this->_vertexDatas[attribType].reserve(attrib.size * NumVertices);
		}
	}
	
	// mesh
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < _entries.size(); i++)
	{
		const aiMesh* paiMesh = _scene->mMeshes[i];
		
		for (unsigned int ii = 0; ii < paiMesh->mNumVertices; ii++)
		{
			//顶点位置
			const aiVector3D* pPos = &paiMesh->mVertices[ii];
			this->_vertexDatas[eShaderVertAttribute_pos].push_back(pPos->x);
			this->_vertexDatas[eShaderVertAttribute_pos].push_back(pPos->y);
			this->_vertexDatas[eShaderVertAttribute_pos].push_back(pPos->z);

			//纹理坐标
			if (flag & MeshAttribStep_texcood)
			{
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][ii]) : &Zero3D;
				this->_vertexDatas[eShaderVertAttribute_texcood].push_back(pTexCoord->x);
				this->_vertexDatas[eShaderVertAttribute_texcood].push_back(pTexCoord->y);
			}
			
			//法线
			if (flag & MeshAttribStep_gen_normal || flag & MeshAttribStep_gen_normal_smooth)
			{
				const aiVector3D* pNormal = &paiMesh->mNormals[ii];
				this->_vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->x);
				this->_vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->y);
				this->_vertexDatas[eShaderVertAttribute_normal].push_back(pNormal->z);
			}

			//切线
			if (flag & MeshAttribStep_tangent)
			{
				const aiVector3D* pTangent = &(paiMesh->mTangents[ii]);
				this->_vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->x);
				this->_vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->y);
				this->_vertexDatas[eShaderVertAttribute_tangent].push_back(pTangent->z);
			}
		}

		if (HaveBone())
		{
			LoadBone(paiMesh, i);
		}

		for (unsigned int ii = 0; ii < paiMesh->mNumFaces; ii++)
		{
			const aiFace& face = paiMesh->mFaces[ii];
			assert(face.mNumIndices == 3);
			this->_indices.push_back(face.mIndices[0]);
			this->_indices.push_back(face.mIndices[1]);
			this->_indices.push_back(face.mIndices[2]);
		}
	}

	return true;
}

void Mesh::GenBuffers()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	for (auto it:_attribs)
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
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW );

	glBindVertexArray(0);
}

void Mesh::BindBufferDatas()
{
	for (auto it : _attribs)
	{
		int attribType = it.first;
		MeshVertexAttrib& attrib = it.second;
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[attribType]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _vertexDatas[attribType].size(), &_vertexDatas[attribType][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribType);
		glVertexAttribPointer(attribType, attrib.size, GL_FLOAT, GL_FALSE, 0, 0);
	}

	_bufferDirty = false;
}

void Mesh::UseBuffers()
{
	if (HaveNormalMap())
	{
		_normalTexture->Bind(NORMAL_TEXTURE);
	}
	
	glBindVertexArray(_vao);

	if (_bufferDirty)
	{
		BindBufferDatas();
	}

	for (uint i = 0; i < _entries.size(); i++)
	{
		const uint MaterialIndex = _entries[i].MaterialIndex;
		
		if (_colorTexture)
		{
			_colorTexture->Bind(COLOR_TEXTURE);
		}
		else
		{
			if (MaterialIndex < _textures.size() && _textures[MaterialIndex])
			{
				_textures[MaterialIndex]->Bind(COLOR_TEXTURE);
			}
			else
			{
				//have not set texture, engine will create one
			}
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, _entries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * _entries[i].BaseIndex), _entries[i].BaseVertex);
	}
	
/*	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);*/

	glBindVertexArray(0);
}

bool Mesh::HaveAttribute(int attrib)
{
	auto it = _attribs.find(attrib);
	if (it == _attribs.end())
	{
		return false;
	}
	return true;
}

void Mesh::CalcNormals()
{
	_vertexDatas[eShaderVertAttribute_normal].resize(_vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < _indices.size(); i += 3)
	{
		unsigned int index0 = _indices[i];
		unsigned int index1 = _indices[i + 1];
		unsigned int index2 = _indices[i + 2];

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

	vector<float>& normals = _vertexDatas[eShaderVertAttribute_normal];
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
	_vertexDatas[eShaderVertAttribute_tangent].resize(_vertexDatas[eShaderVertAttribute_pos].size());
	for (unsigned int i = 0; i < _indices.size(); i += 3)
	{
		unsigned int index0 = _indices[i];
		unsigned int index1 = _indices[i + 1];
		unsigned int index2 = _indices[i + 2];

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
	int& size = _attribs[attrib].size;
	return &_vertexDatas[attrib][vertexIdx * size];
}

void Mesh::SetVertex(int attrib, int vertexIdx, float* pValue)
{
	float* p = GetVertex(attrib, vertexIdx);
	int& attribSize = _attribs[attrib].size;
	for (int i = 0; i < attribSize; i++)
	{
		*(p + i) = *(pValue + i);
	}
}

void Mesh::FillVertexAttributeWithFlag()
{
	vector<float> tempV;
	_sizePerVertex = 0;
	_stridePerVertex = 0;

	_attribs.insert(make_pair(eShaderVertAttribute_pos, MeshVertexAttrib(3, eShaderVertAttribute_pos, _sizePerVertex)));
	_sizePerVertex += 3;
	_vertexDatas.insert(make_pair(eShaderVertAttribute_pos, tempV));

	if (_attribFlag & MeshAttribStep_texcood)
	{
		_attribs.insert(make_pair(eShaderVertAttribute_texcood, MeshVertexAttrib(2, eShaderVertAttribute_texcood, _sizePerVertex)));
		_sizePerVertex += 2;
		_vertexDatas.insert(make_pair(eShaderVertAttribute_texcood, tempV));
	}
	
	if (_attribFlag & MeshAttribStep_gen_normal || _attribFlag & MeshAttribStep_gen_normal_smooth)
	{
		_attribs.insert(make_pair(eShaderVertAttribute_normal, MeshVertexAttrib(3, eShaderVertAttribute_normal, _sizePerVertex)));
		_sizePerVertex += 3;
		_vertexDatas.insert(make_pair(eShaderVertAttribute_normal, tempV));
	}

	if (_attribFlag & MeshAttribStep_tangent)
	{
		_attribs.insert(make_pair(eShaderVertAttribute_tangent, MeshVertexAttrib(3, eShaderVertAttribute_tangent, _sizePerVertex)));
		_sizePerVertex += 3;
		_vertexDatas.insert(make_pair(eShaderVertAttribute_tangent, tempV));
	}

	if (_attribFlag & MeshAttribStep_bone)
	{
		_attribs.insert(make_pair(eShaderVertAttribute_blend_index, MeshVertexAttrib(4, eShaderVertAttribute_blend_index, _sizePerVertex)));
		_sizePerVertex += 4;
		_vertexDatas.insert(make_pair(eShaderVertAttribute_blend_index, tempV));

		_attribs.insert(make_pair(eShaderVertAttribute_blend_weight, MeshVertexAttrib(4, eShaderVertAttribute_blend_weight, _sizePerVertex)));
		_sizePerVertex += 4;
		_vertexDatas.insert(make_pair(eShaderVertAttribute_blend_weight, tempV));
	}

	for (auto it : _attribs)
	{
		_stridePerVertex += it.second.attribSizeBytes;
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
		Texture2D* texture = new Texture2D;
		texture->LoadTextureFromImage(_textureNames[i]);
		_textures.push_back(texture);
	}
}

void Mesh::LoadBone(const aiMesh* paiMesh, uint meshIdx)
{
	for (uint i = 0; i < paiMesh->mNumBones; i++)
	{
		uint boneIdx = 0;
		string boneName(paiMesh->mBones[i]->mName.data);

		if (_boneMapping.find(boneName) == _boneMapping.end())
		{
			boneIdx = _boneNum;
			_boneMapping[boneName] = boneIdx;
			_boneNum++;

			_bonesInfo.push_back(BoneInfo());
			_bonesInfo[boneIdx].BoneOffset = ConvertAiMatrix4x4ToMat4(paiMesh->mBones[i]->mOffsetMatrix);
		}
		else
		{
			boneIdx = _boneMapping[boneName];
		}

		for (uint ii = 0; ii < paiMesh->mBones[i]->mNumWeights; ii++)
		{
			uint vertexID = _entries[meshIdx].BaseVertex + paiMesh->mBones[i]->mWeights[ii].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[ii].mWeight;

			float* blendIndex  = GetVertex(eShaderVertAttribute_blend_index, vertexID);
			float* blendWeight = GetVertex(eShaderVertAttribute_blend_weight, vertexID);
			for (int iii = 0; iii < NUM_BONES_PER_VEREX; iii++)
			{
				if (*(blendWeight+iii) == 0)
				{
					*(blendWeight + iii) = weight;
					*(blendIndex + iii) = boneIdx;
					break;
				}
			}
		}
	}
}

void Mesh::BoneTransform(float timeInSeconds)
{
	float ticksPerSecond = (float)(_scene->mAnimations[0]->mTicksPerSecond != 0 ? _scene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float timeInTicks = timeInSeconds * ticksPerSecond;
	float animationTime = fmod(timeInTicks, (float)_scene->mAnimations[0]->mDuration);

	mat4 identity;
	ReadNodeHeirarchy(animationTime, _scene->mRootNode, identity);
}

void Mesh::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const mat4& parentTransform)
{
	string nodeName(pNode->mName.data);

	const aiAnimation* pAnimation = _scene->mAnimations[0];

	mat4 nodeTransformation = ConvertAiMatrix4x4ToMat4(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);
	if (pNodeAnim)
	{
		aiVector3D scale;
		CalcInterpolatedScaling(scale, animationTime, pNodeAnim);
		mat4 scaleTransform;
		scaleTransform = glm::scale(scaleTransform, vec3(scale.x, scale.y, scale.z));
		
		aiQuaternion rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTime, pNodeAnim);
		mat4 rotationTransform = ConvertAiMatrix3x3ToMat4(rotationQ.GetMatrix());

		aiVector3D translation;
		CalcInterpolatedPosition(translation, animationTime, pNodeAnim);
		mat4 translationTransform;
		translationTransform = glm::translate(translationTransform, vec3(translation.x, translation.y, translation.z));

		nodeTransformation = translationTransform * rotationTransform * scaleTransform;
	}

	mat4 globalTransformation = parentTransform * nodeTransformation;

	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		uint boneIndex = _boneMapping[nodeName];
		_bonesInfo[boneIndex].FinalTransformation = _globalInverseTransform * globalTransformation * _bonesInfo[boneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
	}
}

const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const string nodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
		if (string(pNodeAnim->mNodeName.data) == nodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}

void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) 
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint Mesh::FindPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Mesh::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Mesh::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

void Mesh::SetNormalTexture(Texture2D* texture2D)
{
	SAFE_RELEASE(_normalTexture);
	if (texture2D)
	{
		texture2D->Retain();
	}
	_normalTexture = texture2D;
}

bool Mesh::HaveNormalMap()
{
	return (_attribFlag & MeshAttribStep_tangent) && _normalTexture;
}

bool Mesh::HaveBone()
{
	return (_attribFlag & MeshAttribStep_bone);
}

void Mesh::SetColorTexture(Texture* texture)
{
	SAFE_RELEASE(_colorTexture);
	texture->Retain();
	_colorTexture = texture;
}

Texture* Mesh::GetColorTexture()
{
	return _colorTexture;
}
