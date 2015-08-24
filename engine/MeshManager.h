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

	Vector<Mesh*>* LoadMeshFromFile(const string& fileName, unsigned int flag);

	Vector<Mesh*>* GetMeshs(string fileName);

	void AddMeshsToCache(string fileName, Vector<Mesh*>* meshs);
public:
	map<string, Vector<Mesh*>* > _meshCache;
};