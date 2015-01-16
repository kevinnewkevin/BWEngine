#include "base/Ref.h"
#include "utils/AutoReleasePool.h"


Ref::Ref()
{
}


Ref::~Ref()
{
}

void Ref::retain()
{
	++_refCount;
}

void Ref::release()
{
	--_refCount;
	if (_refCount == 0) {
		delete this;
	}
}

void Ref::autoRelease()
{
	AutoReleasePool::getInstance()->add(this);
}
