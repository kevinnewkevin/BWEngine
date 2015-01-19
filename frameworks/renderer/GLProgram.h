#pragma once
#include "base/Ref.h"
#include "gl/glew.h"
#include "math/mat4.h"


struct VertexAttrib
{
	GLuint index;
	GLint size;
	GLenum type;
	std::string name;
};

struct Uniform
{
	GLint location;
	GLint size;
	GLenum type;
	std::string name;
};

class GLProgram : public Ref
{
	friend class Material;
public:
	GLuint program = 0;
	GLuint vertShader = 0;
	GLuint fragShader = 0;

public:
	struct ATTRIBUTE {
		enum 
		{
			VERTEX_POSITION = 0,
			VERTEX_COLOR,
			VERTEX_TEX_COORD,
			VERTEX_TEX_COORD1,
			VERTEX_TEX_COORD2,
			VERTEX_TEX_COORD3,
			VERTEX_NORMAL,
			VERTEX_MAX,
		};
		static const char* NAME_POSITION	;
		static const char* NAME_COLOR		;
		static const char* NAME_TEX_COORD	;
		static const char* NAME_TEX_COORD1	;
		static const char* NAME_TEX_COORD2	;
		static const char* NAME_TEX_COORD3	;
		static const char* NAME_NORMAL		;
	};

	struct UNIFORM {
		enum
		{
			AMBIENT_COLOR,
			P_MATRIX,
			MV_MATRIX,
			MVP_MATRIX,
			NORMAL_MATRIX,
			TIME,
			RANDOM01,
			SAMPLER0,
			SAMPLER1,
			SAMPLER2,
			SAMPLER3,

			MAX,
		}; 
		static const char* NAME_AMBIENT_COLOR	;
		static const char* NAME_P_MATRIX		;
		static const char* NAME_MV_MATRIX		;
		static const char* NAME_MVP_MATRIX		;
		static const char* NAME_NORMAL_MATRIX	;
		static const char* NAME_TIME			;
		static const char* NAME_SIN_TIME		;
		static const char* NAME_COS_TIME		;
		static const char* NAME_RANDOM01		;
		static const char* NAME_SAMPLER0		;
		static const char* NAME_SAMPLER1		;
		static const char* NAME_SAMPLER2		;
		static const char* NAME_SAMPLER3		;
	};

public:
	static GLProgram* create(const char* shaderFile);

public:
	GLProgram();
	virtual ~GLProgram();

	bool init();
	bool initWithFile(const char* shaderFile);
	bool initWithContent(unsigned char* shader, int size = 0);
	bool initWithFile(const char* frag, const char* vert);
	bool initWithContent(const char* frag, const char* vert);

	const std::string& getName();

	Uniform* getUniform(const std::string& name);
	VertexAttrib* getVertexAttrib(const std::string& name);
	void bindAttribLocation(const std::string& attributeName, GLuint index) const;
	GLint getAttribLocation(const std::string& attributeName);
	GLint getUniformLocation(const std::string& attributeName);
	bool link();
	void use();

public:
	void setUniformsForBuiltins(const Mat4 &mvp);
	void updateUniforms();
	GLint getUniformLocationForName(const char* name) const;

	void setUniformLocationWith1i(GLint location, GLint i1);

	void setUniformLocationWith2i(GLint location, GLint i1, GLint i2);

	void setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3);

	void setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4);

	void setUniformLocationWith2iv(GLint location, GLint* ints, unsigned int numberOfArrays);

	void setUniformLocationWith3iv(GLint location, GLint* ints, unsigned int numberOfArrays);

	void setUniformLocationWith4iv(GLint location, GLint* ints, unsigned int numberOfArrays);

	void setUniformLocationWith1f(GLint location, GLfloat f1);

	void setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2);

	void setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3);

	void setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);

	void setUniformLocationWith1fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays);

	void setUniformLocationWith2fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays);

	void setUniformLocationWith3fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays);

	void setUniformLocationWith4fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays);

	void setUniformLocationWithMatrix2fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);

	void setUniformLocationWithMatrix3fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);

	void setUniformLocationWithMatrix4fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);

protected:
	bool updateUniformLocation(GLint location, const GLvoid* data, unsigned int bytes);

	GLuint loadShaderWithType(GLenum shaderType, const char* source);

	void bindPredefinedVertexAttribs();
	void parseVertexAttribs();
	void parseUniforms();

protected:
	std::string _name;

	struct flag_struct {
		unsigned int usesTime : 1;
		unsigned int usesNormal : 1;
		unsigned int usesMVP : 1;
		unsigned int usesMV : 1;
		unsigned int usesP : 1;
		unsigned int usesRandom : 1;
		flag_struct() { memset(this, 0, sizeof(*this)); }
	} _flags;

	GLint _builtInUniforms[UNIFORM::MAX];
	std::unordered_map<std::string, Uniform> _userUniforms;
	std::unordered_map<std::string, VertexAttrib> _vertexAttribs;
};

