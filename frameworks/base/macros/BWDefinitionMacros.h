#pragma once
#include <assert.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <list>
#include <queue>
#include <algorithm>

#include <thread>

#include <cstdlib>
#include <cstdio>
#include <cctype>

#define SAFE_DELETE(p) if((p)!=nullptr){delete p;p=nullptr;}
#define SAFE_DELETE_ARRAY(p) if((p)!=nullptr){delete [] p;p=nullptr;}
#define SAFE_FREE(p) if((p)!=nullptr){free(p);p=nullptr;}

#define SAFE_RELEASE(p) if((p)!=nullptr){p->release();}
#define SAFE_RELEASE_NULL(p) if((p)!=nullptr){p->release();p=nullptr;}
#define SAFE_RETAIN(p) if((p)!=nullptr){p->retain();}
#define BREAK_IF(cond) if(cond) break

#define ASSERT(p, msg) assert(p)

#define RANDOM_0_1() (fabs(rand()/RAND_MAX))

#ifndef __SSIZE_T
#define __SSIZE_T
typedef int ssize_t;
#endif // __SSIZE_T

#define LOG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define CheckGLError() for (GLint error = glGetError(); error; error = glGetError()) { LOG("OpenGL error 0x%04X in %s %s %d\n", error, __FILE__, __FUNCTION__, __LINE__); }