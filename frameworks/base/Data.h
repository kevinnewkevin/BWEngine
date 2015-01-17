#pragma once
#include "base/Ref.h"

class Data
{
public:
	static const Data Null;

	Data();
	Data(const Data& other);
	Data(Data&& other);
	~Data();

	Data& operator= (const Data& other);
	Data& operator= (Data&& other);

	unsigned char* getBytes() const;

	ssize_t getSize() const;

	void readBytes(unsigned char* out, ssize_t len);

	void copy(const unsigned char* bytes, const ssize_t size);

	void fastSet(unsigned char* bytes, const ssize_t size);

	void clear();

	bool isNull() const;

private:
	void move(Data& other);

private:
	unsigned char* _bytes;
	ssize_t _size;
	ssize_t offset = 0;
};

