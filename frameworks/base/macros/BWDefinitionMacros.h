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
