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

Mesh* MeshManager::LoadMeshFromFile(const string& fileName, uint flag)
{
	Mesh* mesh = GetMesh(fileName);
	if (mesh)
	{
		return mesh;
	}

	mesh = new Mesh;
	mesh->Retain();
	if (mesh->InitFromFile(fileName, flag))
	{
		mesh->GenBuffers();
		AddMeshToCache(fileName, mesh);
		return mesh;
	}
	return NULL;	
}

Mesh* MeshManager::GetMesh(string filename)
{
	auto it = _meshCache.find(filename);
	if (it != _meshCache.end())
	{
		return it->second;
	}
	return NULL;
}

void MeshManager::AddMeshToCache(string fileName, Mesh* mesh)
{
	_meshCache.insert(make_pair(fileName, mesh));
}

