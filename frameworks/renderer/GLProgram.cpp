#include "renderer/GLProgram.h"
#include "utils/FileUtils.h"
#include <regex>

using namespace std;

const char* GLProgram::ATTRIBUTE::NAME_POSITION		= "a_position";
const char* GLProgram::ATTRIBUTE::NAME_COLOR		= "a_color";
const char* GLProgram::ATTRIBUTE::NAME_TEX_COORD	= "a_texCoord";
const char* GLProgram::ATTRIBUTE::NAME_TEX_COORD1	= "a_texCoord1";
const char* GLProgram::ATTRIBUTE::NAME_TEX_COORD2	= "a_texCoord2";
const char* GLProgram::ATTRIBUTE::NAME_TEX_COORD3	= "a_texCoord3";
const char* GLProgram::ATTRIBUTE::NAME_NORMAL		= "a_normal";

const char* GLProgram::UNIFORM::NAME_AMBIENT_COLOR	= "BW_AmbientColor";
const char* GLProgram::UNIFORM::NAME_P_MATRIX		= "BW_PMatrix";
const char* GLProgram::UNIFORM::NAME_MV_MATRIX		= "BW_MVMatrix";
const char* GLProgram::UNIFORM::NAME_MVP_MATRIX		= "BW_MVPMatrix";
const char* GLProgram::UNIFORM::NAME_NORMAL_MATRIX	= "BW_NormalMatrix";
const char* GLProgram::UNIFORM::NAME_TIME			= "BW_Time";
const char* GLProgram::UNIFORM::NAME_SIN_TIME		= "BW_SinTime";
const char* GLProgram::UNIFORM::NAME_COS_TIME		= "BW_CosTime";
const char* GLProgram::UNIFORM::NAME_RANDOM01		= "BW_Random01";
const char* GLProgram::UNIFORM::NAME_SAMPLER0		= "BW_Texture0";
const char* GLProgram::UNIFORM::NAME_SAMPLER1		= "BW_Texture1";
const char* GLProgram::UNIFORM::NAME_SAMPLER2		= "BW_Texture2";
const char* GLProgram::UNIFORM::NAME_SAMPLER3		= "BW_Texture3";

struct ShaderPass {
	string vert;
	string frag;
};

struct ShaderBlock {
	vector<ShaderPass> passes;
};

GLuint GLProgram::loadShaderWithType(GLenum shaderType, const char* source)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader)
	{
		const GLchar *sources[] = {
			"uniform mat4		BW_PMatrix;\n"
			"uniform mat4		BW_MVMatrix;\n"
			"uniform mat4		BW_MVPMatrix;\n"
			"uniform mat3		BW_NormalMatrix;\n"
			"uniform vec4		BW_Time;\n"
			"uniform vec4		BW_SinTime;\n"
			"uniform vec4		BW_CosTime;\n"
			"uniform vec4		BW_Random01;\n"
			"uniform sampler2D	BW_Texture0;\n"
			"uniform sampler2D	BW_Texture1;\n"
			"uniform sampler2D	BW_Texture2;\n"
			"uniform sampler2D	BW_Texture3;\n"
			"//BW INCLUDES END\n\n",
			source,
		};
		glShaderSource(shader, sizeof(sources) / sizeof(*sources), sources, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOG("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

bool GLProgram::initWithContent(const char* pVertexSource, const char* pFragmentSource) 
{
	vertShader = fragShader = program = 0;
	vertShader = loadShaderWithType(GL_VERTEX_SHADER, pVertexSource);
	if (!vertShader) {
		LOG("ERROR: Failed to compile vertex shader");
		return false;
	}

	fragShader = loadShaderWithType(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!fragShader) {
		LOG("ERROR: Failed to compile fragment shader");
		return false;
	}

	program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertShader);
		CheckGLError();
		glAttachShader(program, fragShader);
		CheckGLError();
	}
	CheckGLError();
	return true;
}

const std::string & GLProgram::getName()
{
	return _name;
}

void readSpace(unsigned char* &bytes)
{
	while (isspace(*bytes)) ++bytes;
}

void readContent(unsigned char* &bytes, string& pass, string endWith)
{
	char* ptr = strstr((char*)bytes, endWith.c_str());
	if (ptr != nullptr)
	{
		ssize_t len = ptr - (char*)bytes;
		pass.assign((char*)bytes, len);
		bytes += len;
	}
}

void readPass(unsigned char* &bytes, ShaderPass& pass)
{
	string passName, attrName;
	readSpace(bytes);
	while (!isspace(*bytes)) passName += *bytes++;
	readSpace(bytes);
	if (*bytes != '{') return ; ++bytes; // avoid '{'
	readSpace(bytes);
	while (!isspace(*bytes)) attrName += *bytes++;
	readSpace(bytes);
	if (*bytes != '=') return; ++bytes; // avoid '='
	readSpace(bytes);
	if (bytes[0] != '[' && bytes[1] != '[') return; bytes += 2; // avoid '[['
	if (attrName == "vert") readContent(bytes, pass.vert, "]]");
	else readContent(bytes, pass.frag, "]]");
	bytes += 2; // avoid ']]'

	attrName.assign("");
	readSpace(bytes);
	while (!isspace(*bytes)) attrName += *bytes++;
	readSpace(bytes);
	if (*bytes != '=') return; ++bytes; // avoid '='
	readSpace(bytes);
	if (bytes[0] != '[' && bytes[1] != '[') return; bytes += 2; // avoid '[['
	if (attrName == "vert") readContent(bytes, pass.vert, "]]");
	else readContent(bytes, pass.frag, "]]");
	bytes += 2; // avoid ']]'
	readSpace(bytes);
	if (*bytes != '}') return ; ++bytes; // avoid '}'
}

bool readBlock(unsigned char* &bytes, ShaderBlock& block, unsigned char* end)
{
	readSpace(bytes);
	if (!isalnum(*bytes)) return false;

	string attrName;
	while (!isspace(*bytes)) attrName += *bytes++;
	readSpace(bytes);
	if (attrName == "pass")
	{
		ShaderPass pass;
		readPass(bytes, pass);
		block.passes.push_back(pass);
	}
	return true;
}

bool GLProgram::initWithFile(const char* shaderFile)
{
	Data shader = FileUtils::getInstance()->getDataFromFile(shaderFile);
	unsigned char* bytes = shader.getBytes();
	return initWithContent(bytes, shader.getSize());
}

bool GLProgram::initWithContent(unsigned char* bytes, int size/* = 0*/)
{
	unsigned char* end = size == 0 ? nullptr : bytes + size;
	readSpace(bytes);
	if (*bytes != '"') return 0;  ++bytes; // avoid '"'
	_name = "";
	while (*bytes != '"') _name += *bytes++; ++bytes; // avoid '"'
	readSpace(bytes);
	if (*bytes != ':') return 0; ++bytes; // avoid ':'
	readSpace(bytes);
	if (*bytes != '{') return 0; ++bytes; // avoid '{'
	readSpace(bytes);
	ShaderBlock block;
	while (readBlock(bytes, block, end));
	readSpace(bytes);
	if (*bytes != '}') return 0; ++bytes; // avoid '}'
	return initWithContent(block.passes[0].vert.c_str(), block.passes[0].frag.c_str());
}

bool GLProgram::initWithFile(const char* frag, const char* vert)
{
	string f = FileUtils::getInstance()->getStringFromFile(frag);
	string v = FileUtils::getInstance()->getStringFromFile(vert);
	return initWithContent(f.c_str(), v.c_str());
}

GLProgram * GLProgram::create(const char * shaderFile)
{
	auto ret = new (std::nothrow) GLProgram();
	if (ret && ret->initWithFile(shaderFile)) {
		ret->link();
		ret->updateUniforms();
		ret->autoRelease();
		return ret;
	}

	SAFE_DELETE(ret);
	return nullptr;
}

GLProgram::GLProgram()
{
}


GLProgram::~GLProgram()
{
	if (vertShader)
	{
		glDeleteShader(vertShader);
	}

	if (fragShader)
	{
		glDeleteShader(fragShader);
	}

	vertShader = fragShader = 0;

	if (program)
	{
		glDeleteProgram(program);
	}
}

bool GLProgram::init()
{
	return true;
}

bool GLProgram::link()
{
	ASSERT(program != 0, "Cannot link invalid program");
	GLint status = GL_TRUE;

	bindPredefinedVertexAttribs();

	glLinkProgram(program);

	parseVertexAttribs();
	parseUniforms();

	if (vertShader)
	{
		glDeleteShader(vertShader);
	}

	if (fragShader)
	{
		glDeleteShader(fragShader);
	}

	vertShader = fragShader = 0;
	return (status == GL_TRUE);
}

void GLProgram::use()
{
	glUseProgram(program);
}

void GLProgram::bindPredefinedVertexAttribs()
{
	static const struct {
		const char *attributeName;
		int location;
	} attribute_locations[] =
	{
		{ GLProgram::ATTRIBUTE::NAME_POSITION,		GLProgram::ATTRIBUTE::VERTEX_POSITION },
		{ GLProgram::ATTRIBUTE::NAME_COLOR,			GLProgram::ATTRIBUTE::VERTEX_COLOR },
		{ GLProgram::ATTRIBUTE::NAME_TEX_COORD,		GLProgram::ATTRIBUTE::VERTEX_TEX_COORD },
		{ GLProgram::ATTRIBUTE::NAME_TEX_COORD1,	GLProgram::ATTRIBUTE::VERTEX_TEX_COORD1 },
		{ GLProgram::ATTRIBUTE::NAME_TEX_COORD2,	GLProgram::ATTRIBUTE::VERTEX_TEX_COORD2 },
		{ GLProgram::ATTRIBUTE::NAME_TEX_COORD3,	GLProgram::ATTRIBUTE::VERTEX_TEX_COORD3 },
		{ GLProgram::ATTRIBUTE::NAME_NORMAL,		GLProgram::ATTRIBUTE::VERTEX_NORMAL },
	};

	const int size = sizeof(attribute_locations) / sizeof(attribute_locations[0]);

	for (int i = 0; i<size; i++) 
	{
		glBindAttribLocation(program, attribute_locations[i].location, attribute_locations[i].attributeName);
	}
}

void GLProgram::parseVertexAttribs()
{
	_vertexAttribs.clear();

	// Query and store vertex attribute meta-data from the program.
	GLint activeAttributes;
	GLint length;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
	if (activeAttributes > 0)
	{
		VertexAttrib attribute;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
		if (length > 0)
		{
			GLchar* attribName = (GLchar*)alloca(length + 1);

			for (int i = 0; i < activeAttributes; ++i)
			{
				// Query attribute info.
				glGetActiveAttrib(program, i, length, nullptr, &attribute.size, &attribute.type, attribName);
				attribName[length] = '\0';
				attribute.name.assign(attribName);

				// Query the pre-assigned attribute location
				attribute.index = glGetAttribLocation(program, attribName);
				_vertexAttribs[attribute.name] = attribute;
			}
		}
	}
}

void GLProgram::parseUniforms()
{
	_userUniforms.clear();

	// Query and store uniforms from the program.
	GLint activeUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0)
	{
		GLint length;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
		if (length > 0)
		{
			Uniform uniform;

			GLchar* uniformName = (GLchar*)alloca(length + 1);

			for (int i = 0; i < activeUniforms; ++i)
			{
				// Query uniform info.
				glGetActiveUniform(program, i, length, nullptr, &uniform.size, &uniform.type, uniformName);
				uniformName[length] = '\0';

				// Only add uniforms that are not built-in.
				if (strncmp("BW_", uniformName, 3) != 0) {
					// remove possible array '[]' from uniform name
					if (length > 3)
					{
						char* c = strrchr(uniformName, '[');
						if (c)
						{
							*c = '\0';
						}
					}
					uniform.name = std::string(uniformName);
					uniform.location = glGetUniformLocation(program, uniformName);
					GLenum __gl_error_code = glGetError();
					if (__gl_error_code != GL_NO_ERROR)
					{
						LOG("error: 0x%x", (int)__gl_error_code);
					}
					assert(__gl_error_code == GL_NO_ERROR);
					_userUniforms[uniform.name] = uniform;
				}
			}
		}
	}
}

void GLProgram::setUniformsForBuiltins(const Mat4 & mvp)
{
	if (_flags.usesMVP) {
		setUniformLocationWithMatrix4fv(_builtInUniforms[UNIFORM::MVP_MATRIX], mvp.m, 1);
	}

	if (_flags.usesNormal)
	{
	}

	if (_flags.usesTime) {
		float time = 0;

		setUniformLocationWith4f(_builtInUniforms[UNIFORM::TIME], time / 10.0, time, time * 2, time * 4);
	}

	if (_flags.usesRandom)
		setUniformLocationWith4f(_builtInUniforms[UNIFORM::RANDOM01], RANDOM_0_1(), RANDOM_0_1(), RANDOM_0_1(), RANDOM_0_1());

}

void GLProgram::updateUniforms()
{
	_builtInUniforms[UNIFORM::AMBIENT_COLOR] = glGetUniformLocation(program,	UNIFORM::NAME_AMBIENT_COLOR);
	_builtInUniforms[UNIFORM::P_MATRIX] = glGetUniformLocation(program,			UNIFORM::NAME_P_MATRIX);
	_builtInUniforms[UNIFORM::MV_MATRIX] = glGetUniformLocation(program,		UNIFORM::NAME_MV_MATRIX);
	_builtInUniforms[UNIFORM::MVP_MATRIX] = glGetUniformLocation(program,		UNIFORM::NAME_MVP_MATRIX);
	_builtInUniforms[UNIFORM::NORMAL_MATRIX] = glGetUniformLocation(program,	UNIFORM::NAME_NORMAL_MATRIX);

	_builtInUniforms[UNIFORM::TIME] = glGetUniformLocation(program,		UNIFORM::NAME_TIME);

	_builtInUniforms[UNIFORM::RANDOM01] = glGetUniformLocation(program, UNIFORM::NAME_RANDOM01);

	_builtInUniforms[UNIFORM::SAMPLER0] = glGetUniformLocation(program, UNIFORM::NAME_SAMPLER0);
	_builtInUniforms[UNIFORM::SAMPLER1] = glGetUniformLocation(program, UNIFORM::NAME_SAMPLER1);
	_builtInUniforms[UNIFORM::SAMPLER2] = glGetUniformLocation(program, UNIFORM::NAME_SAMPLER2);
	_builtInUniforms[UNIFORM::SAMPLER3] = glGetUniformLocation(program, UNIFORM::NAME_SAMPLER3);

	_flags.usesP		= _builtInUniforms[UNIFORM::P_MATRIX] != -1;
	_flags.usesMV		= _builtInUniforms[UNIFORM::MV_MATRIX] != -1;
	_flags.usesMVP		= _builtInUniforms[UNIFORM::MVP_MATRIX] != -1;
	_flags.usesNormal	= _builtInUniforms[UNIFORM::NORMAL_MATRIX] != -1;
	_flags.usesTime		= (
		_builtInUniforms[UNIFORM::TIME] != -1
		);
	_flags.usesRandom = _builtInUniforms[UNIFORM::RANDOM01] != -1;

	this->use();

	// Since sample most probably won't change, set it to 0,1,2,3 now.
	if (_builtInUniforms[UNIFORM::SAMPLER0] != -1)
		setUniformLocationWith1i(_builtInUniforms[UNIFORM::SAMPLER0], 0);
	if (_builtInUniforms[UNIFORM::SAMPLER1] != -1)
		setUniformLocationWith1i(_builtInUniforms[UNIFORM::SAMPLER1], 1);
	if (_builtInUniforms[UNIFORM::SAMPLER2] != -1)
		setUniformLocationWith1i(_builtInUniforms[UNIFORM::SAMPLER2], 2);
	if (_builtInUniforms[UNIFORM::SAMPLER3] != -1)
		setUniformLocationWith1i(_builtInUniforms[UNIFORM::SAMPLER3], 3);
}

Uniform* GLProgram::getUniform(const std::string &name)
{
	const auto itr = _userUniforms.find(name);
	if (itr != _userUniforms.end())
		return &itr->second;
	return nullptr;
}

VertexAttrib* GLProgram::getVertexAttrib(const std::string &name)
{
	const auto itr = _vertexAttribs.find(name);
	if (itr != _vertexAttribs.end())
		return &itr->second;
	return nullptr;
}

GLint GLProgram::getUniformLocationForName(const char* name) const
{
	ASSERT(name != nullptr, "Invalid uniform name");
	ASSERT(program != 0, "Invalid operation. Cannot get uniform location when program is not initialized");

	return glGetUniformLocation(program, name);
}

void GLProgram::setUniformLocationWith1i(GLint location, GLint i1)
{
	bool updated = updateUniformLocation(location, &i1, sizeof(i1) * 1);

	if (updated)
	{
		glUniform1i(location, i1);
	}
}

void GLProgram::setUniformLocationWith2i(GLint location, GLint i1, GLint i2)
{
	GLint ints[2] = { i1,i2 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform2i((GLint)location, i1, i2);
	}
}

void GLProgram::setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3)
{
	GLint ints[3] = { i1,i2,i3 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform3i((GLint)location, i1, i2, i3);
	}
}

void GLProgram::setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4)
{
	GLint ints[4] = { i1,i2,i3,i4 };
	bool updated = updateUniformLocation(location, ints, sizeof(ints));

	if (updated)
	{
		glUniform4i((GLint)location, i1, i2, i3, i4);
	}
}

void GLProgram::setUniformLocationWith2iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 2 * numberOfArrays);

	if (updated)
	{
		glUniform2iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith3iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 3 * numberOfArrays);

	if (updated)
	{
		glUniform3iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith4iv(GLint location, GLint* ints, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, ints, sizeof(int) * 4 * numberOfArrays);

	if (updated)
	{
		glUniform4iv((GLint)location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith1f(GLint location, GLfloat f1)
{
	bool updated = updateUniformLocation(location, &f1, sizeof(f1) * 1);

	if (updated)
	{
		glUniform1f((GLint)location, f1);
	}
}

void GLProgram::setUniformLocationWith2f(GLint location, GLfloat f1, GLfloat f2)
{
	GLfloat floats[2] = { f1,f2 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform2f((GLint)location, f1, f2);
	}
}

void GLProgram::setUniformLocationWith3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3)
{
	GLfloat floats[3] = { f1,f2,f3 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform3f((GLint)location, f1, f2, f3);
	}
}

void GLProgram::setUniformLocationWith4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
	GLfloat floats[4] = { f1,f2,f3,f4 };
	bool updated = updateUniformLocation(location, floats, sizeof(floats));

	if (updated)
	{
		glUniform4f((GLint)location, f1, f2, f3, f4);
	}
}

void GLProgram::setUniformLocationWith1fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float)*numberOfArrays);

	if (updated)
	{
		glUniform1fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith2fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 2 * numberOfArrays);

	if (updated)
	{
		glUniform2fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith3fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 3 * numberOfArrays);

	if (updated)
	{
		glUniform3fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith4fv(GLint location, const GLfloat* floats, unsigned int numberOfArrays)
{
	bool updated = updateUniformLocation(location, floats, sizeof(float) * 4 * numberOfArrays);

	if (updated)
	{
		glUniform4fv((GLint)location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWithMatrix2fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices) {
	bool updated = updateUniformLocation(location, matrixArray, sizeof(float) * 4 * numberOfMatrices);

	if (updated)
	{
		glUniformMatrix2fv((GLint)location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

void GLProgram::setUniformLocationWithMatrix3fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices) {
	bool updated = updateUniformLocation(location, matrixArray, sizeof(float) * 9 * numberOfMatrices);

	if (updated)
	{
		glUniformMatrix3fv((GLint)location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

void GLProgram::setUniformLocationWithMatrix4fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices)
{
	bool updated = updateUniformLocation(location, matrixArray, sizeof(float) * 16 * numberOfMatrices);

	if (updated)
	{
		glUniformMatrix4fv((GLint)location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

GLint GLProgram::getAttribLocation(const std::string &attributeName)
{
	return glGetAttribLocation(program, attributeName.c_str());
}

GLint GLProgram::getUniformLocation(const std::string &attributeName)
{
	return glGetUniformLocation(program, attributeName.c_str());
}

void GLProgram::bindAttribLocation(const std::string &attributeName, GLuint index) const
{
	glBindAttribLocation(program, index, attributeName.c_str());
}

bool GLProgram::updateUniformLocation(GLint location, const GLvoid* data, unsigned int bytes)
{
	if (location < 0)
	{
		return false;
	}

	bool updated = true;
	return updated;
}
