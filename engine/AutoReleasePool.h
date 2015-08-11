#pragma once

#include "Ref.h"

#include "vector"
using namespace std;

class AutoReleasePool
{
public:
	static AutoReleasePool* GetInstance();
	static void DestroyInstance();

    AutoReleasePool();
    ~AutoReleasePool();

    void AddRef(Ref* ref);
    void Clear();
   
    bool Contains(Ref* object) const;
private:
    vector<Ref*> _refs;
};
