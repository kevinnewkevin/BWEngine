#include "Texture.h"
#include "Image.h"


bool Texture::initWithImage(Image* i)
{
    if (i== nullptr)
        return false;

    if (i->getMipLevels() > 1)
    {
		// to do mip level
        return true;
    }
    else if (i->isCompressed())
    {
        return true;
    }
	else
	{
		unsigned char* outTempData = nullptr;

		glGenTextures(1, &_name);
		glBindTexture(GL_TEXTURE_2D, _name);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_pixelsWide = i->_width;
		_pixelsHigh = i->_height;
		_contentSize.width = _pixelsWide;
		_contentSize.height = _pixelsHigh;

        glTexImage2D(GL_TEXTURE_2D, 0, i->_internalFormat, (GLsizei)_pixelsWide, (GLsizei)_pixelsHigh, 0, i->_format, i->_type, i->getLevel(0));
		return true;
    }
}

void Texture::begin()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _name);
}

void Texture::end()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getName()
{
	return _name;
}

const Size & Texture::getSize()
{
	return _contentSize;
}
