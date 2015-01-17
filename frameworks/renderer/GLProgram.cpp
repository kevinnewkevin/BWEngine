#include "renderer/GLProgram.h"
#include "utils/FileUtils.h"
#include <regex>

using namespace std;


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
		glShaderSource(shader, 1, &source, nullptr);
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

GLuint GLProgram::initWithContent(const char* pVertexSource, const char* pFragmentSource) 
{
	LOG("%s\n\n%s", pVertexSource, pFragmentSource);
	GLuint vertexShader = loadShaderWithType(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}

	GLuint pixelShader = loadShaderWithType(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexShader);
		CheckGLError("glAttachShader");
		glAttachShader(program, pixelShader);
		CheckGLError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*)malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOG("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
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

GLuint GLProgram::initWithFile(const char* shaderFile)
{
	Data shader = FileUtils::getInstance()->getDataFromFile(shaderFile);
	unsigned char* bytes = shader.getBytes();
	return initWithContent(bytes, shader.getSize());
}

GLuint GLProgram::initWithContent(unsigned char* bytes, int size/* = 0*/)
{
	unsigned char* end = size == 0 ? nullptr : bytes + size;
	readSpace(bytes);
	if (*bytes != '"') return 0;  ++bytes; // avoid '"'
	string shaderName;
	while (*bytes != '"') shaderName += *bytes++; ++bytes; // avoid '"'
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

GLuint GLProgram::initWithFile(const char* frag, const char* vert)
{
	string f = FileUtils::getInstance()->getStringFromFile(frag);
	string v = FileUtils::getInstance()->getStringFromFile(vert);
	return initWithContent(f.c_str(), v.c_str());
}

GLProgram::GLProgram()
{
}


GLProgram::~GLProgram()
{
}

void GLProgram::init()
{
	program = initWithFile("shaders/default.shader");
	
	worldLocation = glGetUniformLocation(program, "MVP");
	sampler	= glGetUniformLocation(program, "texture1");

	glActiveTexture(GL_TEXTURE0);
	glBindAttribLocation(program, 0, "vPosition");
	glBindAttribLocation(program, 1, "vTexCoord");
}