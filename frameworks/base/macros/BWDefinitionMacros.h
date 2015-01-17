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
#define SAFE_RETAIN(p) if((p)!=nullptr){p->retain();}
#define BREAK_IF(cond) if(cond) break

#define ASSERT(p, msg) assert(p)

#ifndef __SSIZE_T
#define __SSIZE_T
typedef int ssize_t;
#endif // __SSIZE_T

#define LOG(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#define CheckGLError(msg) for (GLint error = glGetError(); error; error = glGetError()) { LOG("err"); }