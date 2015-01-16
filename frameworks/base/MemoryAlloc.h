#pragma once
#include "Types.h"

extern u32 gMemoryAlloc;

template<typename T>
inline T* new_track(u32 count)
{
	gMemoryAlloc += sizeof(T)*count;
	return reinterpret_cast<T*>(operator new(count*sizeof(T))); 
}