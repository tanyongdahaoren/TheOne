#include "Ref.h"
#include "AutoReleasePool.h"

Ref::Ref()
	: _referenceCount(1)
{
	AutoReleasePool::GetInstance()->AddRef(this);
}

Ref::~Ref()
{

}

void Ref::Retain()
{
	 ++_referenceCount;
}

void Ref::Release()
{
	--_referenceCount;

	if (_referenceCount == 0 && !AutoReleasePool::GetInstance()->Contains(this))
	{
		delete this;
	}
}
