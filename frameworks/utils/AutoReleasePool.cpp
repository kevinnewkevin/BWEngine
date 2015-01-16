#include "AutoReleasePool.h"

static AutoReleasePool* gs_instance = nullptr;

AutoReleasePool * AutoReleasePool::getInstance()
{
	if (gs_instance == nullptr)
	{
		gs_instance = new AutoReleasePool();
	}
	return gs_instance;
}

void AutoReleasePool::purge()
{
	SAFE_DELETE(gs_instance);
}

AutoReleasePool::AutoReleasePool()
{
}


AutoReleasePool::~AutoReleasePool()
{
}

void AutoReleasePool::add(Ref * obj)
{
	assert(obj != nullptr);
	_list.push_back(obj);
}

void AutoReleasePool::remove(Ref * obj)
{
	return;
	// not to support
	assert(obj != nullptr);
	_list.remove(obj);
}

void AutoReleasePool::clear()
{
	auto iter = _list.begin();
	while (iter != _list.end()) {
		(*iter)->release();
		++iter;
	}
}
