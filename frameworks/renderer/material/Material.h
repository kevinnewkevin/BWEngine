#pragma once
#include "base/Ref.h"

class GLProgram;
class Texture;
struct Uniform;
struct VertexAttrib;

class UniformValue
{
	friend class GLProgram;
	friend class Material;
public:
	UniformValue();
	UniformValue(Uniform *uniform, GLProgram* glprogram);
	~UniformValue();

	void setFloat(float value);
	void setInt(int value);
	void setVec2(const Vec2& value);
	void setVec3(const Vec3& value);
	void setVec4(const Vec4& value);
	void setMat4(const Mat4& value);
	void setCallback(const std::function<void(GLProgram*, Uniform*)> &callback);
	void setTexture(GLuint textureId, GLuint activeTexture);

	void apply();

protected:
	Uniform* _uniform;  // weak ref
	GLProgram* _glprogram; // weak ref
	bool _useCallback;

	union U {
		float floatValue;
		int intValue;
		float v2Value[2];
		float v3Value[3];
		float v4Value[4];
		float matrixValue[16];
		struct {
			GLuint textureId;
			GLuint textureUnit;
		} tex;
		std::function<void(GLProgram*, Uniform*)> *callback;

		U() { memset(this, 0, sizeof(*this)); }
		~U() {}
		U& operator=(const U& other) {
			memcpy(this, &other, sizeof(*this));
			return *this;
		}
	} _value;
};

//
//
// VertexAttribValue
//
//
class VertexAttribValue
{
	friend class GLProgram;
	friend class Material;

public:
	VertexAttribValue(VertexAttrib *vertexAttrib);
	VertexAttribValue();
	~VertexAttribValue();

	void setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer);
	void setCallback(const std::function<void(VertexAttrib*)> &callback);
	void apply();

protected:
	VertexAttrib* _vertexAttrib;  // weak ref
	bool _useCallback;
	bool _enabled;

	union U {
		struct {
			GLint size;
			GLenum type;
			GLboolean normalized;
			GLsizei stride;
			GLvoid *pointer;
		} pointer;
		std::function<void(VertexAttrib*)> *callback;

		U() { memset(this, 0, sizeof(*this)); }
		~U() {}
		U& operator=(const U& other) {
			memcpy(this, &other, sizeof(*this));
			return *this;
		}
	} _value;
};


/**
GLProgramState holds the 'state' (uniforms and attributes) of the GLProgram.
A GLProgram can be used by thousands of Nodes, but if different uniform values
are going to be used, then each node will need its own GLProgramState
*/
class Material : public Ref
{
	friend class MaterialCache;
public:

	/** returns a new instance of GLProgramState for a given GLProgram */
	static Material* create(GLProgram* glprogram);

	// apply GLProgram, attributes and uniforms
	void apply(const Mat4& modelView);

	void applyGLProgram(const Mat4& modelView);
	/**
	* apply vertex attributes
	* @param applyAttribFlags Call GL::enableVertexAttribs(_vertexAttribsFlags) or not
	*/
	void applyAttributes(bool applyAttribFlags = true);
	void applyUniforms();

	void setGLProgram(GLProgram* glprogram);
	GLProgram* getGLProgram() const { return _glprogram; }

	// vertex attribs
	uint32_t getVertexAttribsFlags() const { return _vertexAttribsFlags; }
	ssize_t getVertexAttribCount() const { return _attributes.size(); }
	void setVertexAttribCallback(const std::string &name, const std::function<void(VertexAttrib*)> &callback);
	void setVertexAttribPointer(const std::string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer);

	// user defined uniforms
	ssize_t getUniformCount() const { return _uniforms.size(); }
	void setUniformInt(const std::string &uniformName, int value);
	void setUniformFloat(const std::string &uniformName, float value);
	void setUniformVec2(const std::string &uniformName, const Vec2& value);
	void setUniformVec3(const std::string &uniformName, const Vec3& value);
	void setUniformVec4(const std::string &uniformName, const Vec4& value);
	void setUniformMat4(const std::string &uniformName, const Mat4& value);
	void setUniformCallback(const std::string &uniformName, const std::function<void(GLProgram*, Uniform*)> &callback);
	void setUniformTexture(const std::string &uniformName, Texture *texture);
	void setUniformTexture(const std::string &uniformName, GLuint textureId);

	void setUniformInt(GLint uniformLocation, int value);
	void setUniformFloat(GLint uniformLocation, float value);
	void setUniformVec2(GLint uniformLocation, const Vec2& value);
	void setUniformVec3(GLint uniformLocation, const Vec3& value);
	void setUniformVec4(GLint uniformLocation, const Vec4& value);
	void setUniformMat4(GLint uniformLocation, const Mat4& value);
	void setUniformCallback(GLint uniformLocation, const std::function<void(GLProgram*, Uniform*)> &callback);
	void setUniformTexture(GLint uniformLocation, Texture *texture);
	void setUniformTexture(GLint uniformLocation, GLuint textureId);

protected:
	Material();
	~Material();
	bool init(GLProgram* program);
	void resetGLProgram();
	VertexAttribValue* getVertexAttribValue(const std::string &attributeName);
	UniformValue* getUniformValue(const std::string &uniformName);
	UniformValue* getUniformValue(GLint uniformLocation);

	bool _uniformAttributeValueDirty;
	std::unordered_map<std::string, GLint> _uniformsByName;
	std::unordered_map<GLint, UniformValue> _uniforms;
	std::unordered_map<std::string, VertexAttribValue> _attributes;
	std::unordered_map<std::string, int> _boundTextureUnits;

	int _textureUnitIndex;
	uint32_t _vertexAttribsFlags;
	GLProgram *_glprogram;
};