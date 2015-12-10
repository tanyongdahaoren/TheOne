#include "AutoReleasePool.h"
#include "Defines.h"


static AutoReleasePool* s_instance = nullptr;
AutoReleasePool* AutoReleasePool::GetInstance()
{
	if (! s_instance)
	{
		s_instance = new AutoReleasePool();
	}

	return s_instance;
}

void AutoReleasePool::DestroyInstance()
{
	SAFE_DELETE(s_instance);
}

AutoReleasePool::AutoReleasePool()
{
	_refs.reserve(150);
}

AutoReleasePool::~AutoReleasePool()
{
	Clear();
}

void AutoReleasePool::AddRef(Ref* object)
{
	_refs.push_back(object);
}

void AutoReleasePool::Clear()
{
	std::vector<Ref*> releasings;
	releasings.swap(_refs);
	for (const auto &ref : releasings)
	{
		ref->Release();
	}
}

bool AutoReleasePool::Contains(Ref* ref) const
{
	for (const auto& obj : _refs)
	{
		if (obj == ref)
			return true;
	}
	return false;
}