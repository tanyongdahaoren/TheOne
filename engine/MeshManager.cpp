#include "MeshManager.h"
#include "Defines.h"
#include "ShaderValue.h"

static MeshManager* s_instance = nullptr;
MeshManager* MeshManager::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new MeshManager();
	}

	return s_instance;
}

void MeshManager::DestroyInstance()
{
	SAFE_DELETE(s_instance);
}

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{

}

Vector<Mesh*>* MeshManager::LoadMeshFromFile(const string& fileName, unsigned int flag)
{
	Vector<Mesh*>* meshs = GetMeshs(fileName);
	if (meshs)
	{
		return meshs;
	}

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(fileName.c_str(), flag);

	if (!pScene)
	{
		printf("Error parsing '%s': '%s'\n", fileName.c_str(), importer.GetErrorString());
		return NULL;
	}
	else
	{
		Vector<Mesh*>* meshs = new Vector < Mesh* > ;

		//textures
		vector<string> textureNames;
		textureNames.resize(pScene->mNumMaterials);

		for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
		{
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					textureNames[i] = Path.data;
				}
			}
		}

		int sizePerVertex = 0;
		map<int, MeshVertexAttrib> atts;
		atts.insert(make_pair(eShaderVertAttribute_pos,
			MeshVertexAttrib(3, eShaderVertAttribute_pos, sizePerVertex)));
		sizePerVertex += 3;
		
		atts.insert(make_pair(eShaderVertAttribute_texcood,
			MeshVertexAttrib(2, eShaderVertAttribute_texcood, sizePerVertex)));
		sizePerVertex += 2;

		atts.insert(make_pair(eShaderVertAttribute_normal,
			MeshVertexAttrib(3, eShaderVertAttribute_normal, sizePerVertex)));
		sizePerVertex += 3;
		
		if (flag & aiProcess_CalcTangentSpace)
		{
			atts.insert(make_pair(eShaderVertAttribute_tangent,
				MeshVertexAttrib(3, eShaderVertAttribute_tangent, sizePerVertex)));
			sizePerVertex += 3;
		}

		int stridePerVertex = 0;
		for (auto it : atts)
		{
			stridePerVertex += it.second.attribSizeBytes;
		}

		// meshs
		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];

			Mesh* oneMesh = new Mesh;

			oneMesh->_textureName = textureNames[paiMesh->mMaterialIndex];

			oneMesh->attribs = atts;

			oneMesh->stridePerVertex = stridePerVertex;

			oneMesh->sizePerVertex = sizePerVertex;

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
			{
				//顶点位置
				const aiVector3D* pPos = &paiMesh->mVertices[i];
				oneMesh->vertices.push_back(pPos->x);
				oneMesh->vertices.push_back(pPos->y);
				oneMesh->vertices.push_back(pPos->z);

				//纹理坐标
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
				oneMesh->vertices.push_back(pTexCoord->x);
				oneMesh->vertices.push_back(pTexCoord->y);

				//如果模型有法线 则使用模型法线 否则 后续进行计算
				if (!VERTEX_CAL_NORMAL)
				{
					const aiVector3D* pNormal = &paiMesh->mNormals[i];
					oneMesh->vertices.push_back(pNormal->x);
					oneMesh->vertices.push_back(pNormal->y);
					oneMesh->vertices.push_back(pNormal->z);
				}
				else
				{
					oneMesh->vertices.push_back(0);
					oneMesh->vertices.push_back(0);
					oneMesh->vertices.push_back(0);
				}

				//切线
				if (flag & aiProcess_CalcTangentSpace)
				{
					if (!VERTEX_CAL_TANGENT)
					{
						const aiVector3D* pTangent = &(paiMesh->mTangents[i]);
						oneMesh->vertices.push_back(pTangent->x);
						oneMesh->vertices.push_back(pTangent->y);
						oneMesh->vertices.push_back(pTangent->z);
					}
					else
					{
						oneMesh->vertices.push_back(0);
						oneMesh->vertices.push_back(0);
						oneMesh->vertices.push_back(0);
					}
				}
			}

			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
			{
				const aiFace& face = paiMesh->mFaces[i];
				assert(face.mNumIndices == 3);
				oneMesh->indices.push_back(face.mIndices[0]);
				oneMesh->indices.push_back(face.mIndices[1]);
				oneMesh->indices.push_back(face.mIndices[2]);
			}

			//根据顶点计算法线
			if (VERTEX_CAL_NORMAL)
			{
				oneMesh->CalcNormals();
			}
			//需要切线 并且要计算
			if ((flag & aiProcess_CalcTangentSpace) && VERTEX_CAL_TANGENT)
			{
				oneMesh->CalcTangents();
			}
			
			meshs->push_back(oneMesh);
		}

		AddMeshsToCache(fileName, meshs);
		return GetMeshs(fileName);
	}	
}

Vector<Mesh*>* MeshManager::GetMeshs(string filename)
{
	auto it = _meshCache.find(filename);
	if (it != _meshCache.end())
	{
		return it->second;
	}
	return NULL;
}

void MeshManager::AddMeshsToCache(string fileName, Vector<Mesh*>* meshs)
{
	_meshCache.insert(make_pair(fileName, meshs));
}

