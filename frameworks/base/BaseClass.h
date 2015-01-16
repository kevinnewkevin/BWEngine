#pragma once
#include "macros/BWMacros.h"


typedef struct _msgMethod {
	enum class TYPE {
		NONE,
		FUNC,
		CLASS,
	};

	TYPE type;

	_msgMethod()
	{
		type = TYPE::NONE;
	}
}MSGMethod;

struct _msgQueueIndex {
	unsigned int _0 : 1, _1 : 1, _2 : 1, _3 : 1, _4 : 1, _5 : 1, _6 : 1, _7 : 1, _8 : 1, _9 : 1, _99 : 7;
	unsigned int A : 1, B : 1, C : 1, D : 1, E : 1, F : 1, G : 1, H : 1, I : 1, J : 1, K : 1, L : 1, M : 1, N : 1, O : 1, P : 1;
	unsigned int Q : 1, R : 1, S : 1, T : 1, U : 1, V : 1, W : 1, X : 1, Y : 1, Z : 1, _ZZ : 4, _ : 1, __ : 1;
	unsigned int a : 1, b : 1, c : 1, d : 1, e : 1, f : 1, g : 1, h : 1, i : 1, j : 1, k : 1, l : 1, m : 1, n : 1, o : 1, p : 1;
	unsigned int q : 1, r : 1, s : 1, t : 1, u : 1, v : 1, w : 1, x : 1, y : 1, z : 1;
};

typedef struct _msgPool {
	_msgPool() {
		memset(index, 0, sizeof(index));
	}



	_msgQueueIndex index[20];
}MSGPool;

class BaseMSGClass
{
public:
	BaseMSGClass();
	virtual ~BaseMSGClass();

public:
	bool sendMessage(const char* msg, std::vector<std::string>& params);

	void receiveMessage(const char* msg);
protected:
	MSGPool _msgPool;
};

