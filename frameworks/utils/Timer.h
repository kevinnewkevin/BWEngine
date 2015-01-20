#pragma once
#include "base/Ref.h"
#include <Windows.h>

class Timer : public Ref
{
public:
	static float clock()
	{
		struct timeval val;
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);
		val.tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val.tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val.tv_sec * 1000000.0);
		float ret = val.tv_sec + val.tv_usec / 1000000.0f;
		return ret;
	}
};

