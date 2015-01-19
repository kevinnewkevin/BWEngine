#pragma once
#include "base/macros/BWMacros.h"


static uint32_t s_attributeFlags = 0;

class GL
{
public:
	enum {
		VERTEX_ATTRIB_FLAG_NONE			= 0,

		VERTEX_ATTRIB_FLAG_POSITION		= 1 << 0,
		VERTEX_ATTRIB_FLAG_COLOR		= 1 << 1,
		VERTEX_ATTRIB_FLAG_TEX_COORD	= 1 << 2,
		VERTEX_ATTRIB_FLAG_NORMAL		= 1 << 3,

		VERTEX_ATTRIB_FLAG_POS_COLOR_TEX = (VERTEX_ATTRIB_FLAG_POSITION | VERTEX_ATTRIB_FLAG_COLOR | VERTEX_ATTRIB_FLAG_TEX_COORD),
	};

	static void bindTexture2D(GLuint textureId)
	{
		GL::bindTexture2DN(0, textureId);
	}

	static void deleteTexture(GLuint textureId)
	{
		glDeleteTextures(1, &textureId);
	}

	static void activeTexture(GLenum texture)
	{
		glActiveTexture(texture);
	}

	static void bindTexture2DN(GLuint textureUnit, GLuint textureId)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	static void SetBlending(GLenum sfactor, GLenum dfactor)
	{
		if (sfactor == GL_ONE && dfactor == GL_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFunc(sfactor, dfactor);
		}
	}

	static void blendFunc(GLenum sfactor, GLenum dfactor)
	{
		SetBlending(sfactor, dfactor);
	}

	static void useProgram(GLuint program)
	{
		glUseProgram(program);
	}

	static void deleteProgram(GLuint program)
	{
		glDeleteProgram(program);
	}

	static void enableVertexAttribs(uint32_t flags)
	{
		for (int i = 0; i < 16; i++) {
			unsigned int bit = 1 << i;
			bool enabled = flags & bit;
			bool enabledBefore = s_attributeFlags & bit;
			if (enabled != enabledBefore) {
				if (enabled)
					glEnableVertexAttribArray(i);
				else
					glDisableVertexAttribArray(i);
			}
		}
		s_attributeFlags = flags;
	}
};

