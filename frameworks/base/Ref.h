#pragma once
#include "base/BaseClass.h"

class Ref
{
public:
	Ref();
	virtual ~Ref();

public:
	virtual void retain();

	virtual void release();

	virtual void autoRelease();

protected:
	int _refCount = 1;
};

