#include "renderer/GLProgram.h"

	static const char gVertexShader[] =
		"attribute vec2 vTexCoord;\n"
		"attribute vec4 vPosition;\n"
		"uniform mat4 WVP;\n"
		"varying vec2 v_texCoord;\n"
		"void main() {\n"
		"  gl_Position = WVP*vPosition;\n"
		"  v_texCoord = vTexCoord;\n"
    "}\n";

	static const char gFragmentShader[] =
		"precision mediump float;\n"
		"uniform sampler2D texture1;"
		"varying vec2 v_texCoord;\n"
		"void main() {\n"
		"   gl_FragColor = texture2D(texture1, v_texCoord);\n"
		//"   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
		"}\n";
	
	void uglCheckError(const char* op) {
		for (GLint error = glGetError(); error; error
			= glGetError()) {
		}
	}

GLuint uglLoadShader(GLenum shaderType, const char* source)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader)
	{
		glShaderSource(shader, 1, &source, NULL);
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
					LOG("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint uglCreateProgram(const char* pVertexSource, const char* pFragmentSource) {
	GLuint vertexShader = uglLoadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}

	GLuint pixelShader = uglLoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexShader);
		uglCheckError("glAttachShader");
		glAttachShader(program, pixelShader);
		uglCheckError("glAttachShader");
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


GLProgram::GLProgram()
{
}


GLProgram::~GLProgram()
{
}

void GLProgram::init()
{
	program = uglCreateProgram(gVertexShader, gFragmentShader);
	
	worldLocation = glGetUniformLocation(program, "WVP");
	sampler	= glGetUniformLocation(program, "texture1");

	glActiveTexture(GL_TEXTURE0);
	glBindAttribLocation(program, 0, "vPosition");
	glBindAttribLocation(program, 1, "vTexCoord");
}