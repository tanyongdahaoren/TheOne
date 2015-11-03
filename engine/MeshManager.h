#pragma once

#include <Importer.hpp> // C++ importer interface
#include <scene.h>      // Output data structure
#include <postprocess.h> // Post processing flags

#include "Ref.h"
#include "Mesh.h"
#include <vector>
#include <map>
#include "Vector.h"

using namespace std;

class MeshManager
{
public:
	static MeshManager* GetInstance();

	static void DestroyInstance();

	MeshManager();

	~MeshManager();

	Mesh* LoadMeshFromFile(const string& fileName, bool skelon, bool tangent);

	Mesh* GetMesh(string fileName);

	void AddMeshToCache(string fileName, Mesh* meshs);
public:
	map<string, Mesh*> _meshCache;
};