#pragma once

#include <vector>
#include <assert.h>

#include <GL/glew.h>

class Image {
public:

    Image();
    virtual ~Image();

    int getWidth() const { return _width; }

    int getHeight() const { return _height; }

    int getDepth() const { return _depth; }

    int getMipLevels() const { return _levelCount; }

    int getFaces() const { return _faces; }

    GLenum getFormat() const { return _format; }

    GLenum getInternalFormat() const { return _internalFormat; }

    GLenum getType() const { return _type; }

    int getImageSize(int level = 0) const;

    bool isCompressed() const {
        switch(_format) {
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			case GL_COMPRESSED_LUMINANCE_LATC1_EXT:
			case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT:
			case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT:
			case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT:
                return true;
        }
        return false;
    }

    bool isCubeMap() const { return _faces > 0; }

    bool isVolume() const { return _depth > 0; }

    const void* getLevel( int level, GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X) const;
    void* getLevel( int level, GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X);

    bool convertCrossToCubemap();

    bool setImage( int width, int height, GLenum format, GLenum type, const void* data);

	void flipSurface();
public:
    int _width;
    int _height;
    int _depth;
    int _levelCount;
    int _faces;
    GLenum _format;
    GLenum _internalFormat;
    GLenum _type;
    int _elementSize;

    //pointers to the levels
    std::vector<GLubyte*> _data;

    void freeData();
};
