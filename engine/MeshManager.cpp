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

		vector<MeshVertexAttrib> atts;
		atts.push_back(MeshVertexAttrib(3, eShaderVertAttribute_pos));
		atts.push_back(MeshVertexAttrib(2, eShaderVertAttribute_texcood));
		atts.push_back(MeshVertexAttrib(3, eShaderVertAttribute_normal));
		if (flag & aiProcess_CalcTangentSpace)
		{
			atts.push_back(MeshVertexAttrib(3, eShaderVertAttribute_tangent));
		}

		int sizePerVertex = 0;
		for (int i = 0; i < atts.size(); i++)
		{
			sizePerVertex += atts[i].attribSizeBytes;
		}

		// meshs
		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];

			Mesh* oneMesh = new Mesh;

			oneMesh->_textureName = textureNames[paiMesh->mMaterialIndex];

			oneMesh->attribs = atts;

			oneMesh->sizePerVertex = sizePerVertex;

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
			{
				const aiVector3D* pPos = &paiMesh->mVertices[i];
				const aiVector3D* pNormal = &paiMesh->mNormals[i];
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

				MeshVertex v;
				v.oneVertex.push_back(pPos->x);
				v.oneVertex.push_back(pPos->y);
				v.oneVertex.push_back(pPos->z);

				v.oneVertex.push_back(pTexCoord->x);
				v.oneVertex.push_back(pTexCoord->y);

				v.oneVertex.push_back(pNormal->x);
				v.oneVertex.push_back(pNormal->y);
				v.oneVertex.push_back(pNormal->z);

				oneMesh->vertices.push_back(v);
			}

			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
			{
				const aiFace& face = paiMesh->mFaces[i];
				assert(face.mNumIndices == 3);
				oneMesh->indices.push_back(face.mIndices[0]);
				oneMesh->indices.push_back(face.mIndices[1]);
				oneMesh->indices.push_back(face.mIndices[2]);
			}

			//oneMesh->CalcNormals();

			oneMesh->GenBuffers();

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

