#pragma once

#define SAFE_RELEASE(p) do { if(p) { (p)->Release(); } } while(0)

class Ref
{
protected:
	Ref();
	virtual ~Ref();
public:
	void Retain();
	void Release();
protected:
	unsigned int _referenceCount;
};