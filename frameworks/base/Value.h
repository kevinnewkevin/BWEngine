#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "math/vec2.h"

class Value;

typedef std::vector<Value> ValueVector;
typedef std::unordered_map<std::string, Value> ValueMap;
typedef std::unordered_map<int, Value> ValueMapIntKey;

extern const ValueVector ValueVectorNull;
extern const ValueMap ValueMapNull;
extern const ValueMapIntKey ValueMapIntKeyNull;

class Value
{
public:
    static const Value Null;

    Value();
    explicit Value(unsigned char v);
    explicit Value(int v);
    explicit Value(float v);
    explicit Value(double v);
    explicit Value(bool v);
    explicit Value(const char* v);
    explicit Value(const std::string& v);

    explicit Value(const ValueVector& v);
    explicit Value(ValueVector&& v);

    explicit Value(const ValueMap& v);
    explicit Value(ValueMap&& v);

    explicit Value(const ValueMapIntKey& v);
    explicit Value(ValueMapIntKey&& v);

    Value(const Value& other);
    Value(Value&& other);
    ~Value();

    // assignment operator
    Value& operator= (const Value& other);
    Value& operator= (Value&& other);

    Value& operator= (unsigned char v);
    Value& operator= (int v);
    Value& operator= (float v);
    Value& operator= (double v);
    Value& operator= (bool v);
    Value& operator= (const char* v);
    Value& operator= (const std::string& v);

    Value& operator= (const ValueVector& v);
    Value& operator= (ValueVector&& v);

    Value& operator= (const ValueMap& v);
        Value& operator= (ValueMap&& v);

    Value& operator= (const ValueMapIntKey& v);
    Value& operator= (ValueMapIntKey&& v);

    // equal operator
    bool operator!= (const Value& v);
    bool operator!= (const Value& v) const;
    bool operator== (const Value& v);
    bool operator== (const Value& v) const;

    unsigned char asByte() const;
    int asInt() const;
    float asFloat() const;
    double asDouble() const;
    bool asBool() const;
    std::string asString() const;

    ValueVector& asValueVector();
    const ValueVector& asValueVector() const;

    ValueMap& asValueMap();
    const ValueMap& asValueMap() const;

    ValueMapIntKey& asIntKeyMap();
    const ValueMapIntKey& asIntKeyMap() const;

    inline bool isNull() const { return _type == Type::NONE; }

    enum class Type
    {
        NONE = 0,
        BYTE,
        INTEGER,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        STRING,
        VECTOR,
        MAP,
        INT_KEY_MAP
    };

    inline Type getType() const { return _type; };

    std::string getDescription();

private:
    void clear();
    void reset(Type type);

    union
    {
        unsigned char byteVal;
        int intVal;
        float floatVal;
        double doubleVal;
        bool boolVal;

        std::string* strVal;
        ValueVector* vectorVal;
        ValueMap* mapVal;
        ValueMapIntKey* intKeyMapVal;
    }_field;

    Type _type;
};

struct Size {
	float width = 0, height = 0;
	Size(){}
	Size(float w, float h) : width(w), height(h) {}
	Size(const Size& s) : width(s.width), height(s.height) {}
	Size(const Vec2& v) : width(v.x), height(v.y) {}
};

struct Rect {
	float x = 0, y = 0;
	float width = 0, height = 0;
	Rect(){}
	Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}
	Rect(const Rect& v) : x(v.x), y(v.y), width(v.width), height(v.height) {}
	Rect(const Vec2& v, const Size& s) : x(v.x), y(v.y), width(s.width), height(s.height) {}
};