#pragma once
#include "base/Ref.h"
#include "base/Vector.h"

class AutoReleasePool
{
public:
	static AutoReleasePool* getInstance();
	static void purge();

	AutoReleasePool();
	virtual ~AutoReleasePool();

	void add(Ref* obj);
	void remove(Ref* obj);

	void clear();
protected:
	std::list<Ref*> _list;
};

