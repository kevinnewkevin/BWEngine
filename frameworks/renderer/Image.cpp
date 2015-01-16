#include <string.h>
#include <algorithm>

#include "Image.h"

using std::vector;
using std::max;

Image::Image() : _width(0), _height(0), _depth(0), _levelCount(0), _faces(0), _format(GL_RGBA),
    _internalFormat(GL_RGBA8), _type(GL_UNSIGNED_BYTE), _elementSize(0) {
}

Image::~Image() {
    freeData();
}

void Image::freeData() {
    for (vector<GLubyte*>::iterator it = _data.begin(); it != _data.end(); it++) {
        delete []*it;
    }
    _data.clear();
}

int Image::getImageSize( int level) const {
    bool compressed = isCompressed();
    int w = _width >> level;
    int h = _height >> level;
    int d = _depth >> level;
    w = (w) ? w : 1;
    h = (h) ? h : 1;
    d = (d) ? d : 1;
    int bw = (compressed) ? ( w + 3 ) / 4 : w;
    int bh = (compressed) ? ( h + 3 ) / 4 : h;
    int elementSize = _elementSize;

    return bw*bh*d*elementSize;
}

const void* Image::getLevel( int level, GLenum face) const {
    assert( level < _levelCount);
    assert( _faces == 0 || ( face >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));

    face = face - GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    assert( (face*_levelCount + level) < (int)_data.size());
    return _data[ face*_levelCount + level];
}

void* Image::getLevel( int level, GLenum face) {
    assert( level < _levelCount);
    assert( _faces == 0 || ( face >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));

    face = face - GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    assert( (face*_levelCount + level) < (int)_data.size());
    return _data[ face*_levelCount + level];
}

bool Image::convertCrossToCubemap() {
    //can't already be a cubemap
    if (isCubeMap())
        return false;

    //mipmaps are not supported
    if (_levelCount != 1)
        return false;

    //compressed textures are not supported
    if (isCompressed())
        return false;

    //this function only supports vertical cross format for now (3 wide by 4 high)
    if (  (_width / 3 != _height / 4) || (_width % 3 != 0) || (_height % 4 != 0) || (_depth != 0))
        return false;

    //get the source data
    GLubyte *data = _data[0];

    int fWidth = _width / 3;
    int fHeight = _height / 4;

    //remove the old pointer from the vector
    _data.pop_back();
    
    GLubyte *face = new GLubyte[ fWidth * fHeight * _elementSize];
    GLubyte *ptr;

    //extract the faces

    // positive X
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        memcpy( ptr, &data[((_height - (fHeight + j + 1))*_width + 2 * fWidth) * _elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }
    _data.push_back(face);

    // negative X
    face = new GLubyte[ fWidth * fHeight * _elementSize];
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        memcpy( ptr, &data[(_height - (fHeight + j + 1))*_width*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }
    _data.push_back(face);

    // positive Y
    face = new GLubyte[ fWidth * fHeight * _elementSize];
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        memcpy( ptr, &data[((4 * fHeight - j - 1)*_width + fWidth)*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }
    _data.push_back(face);

    // negative Y
    face = new GLubyte[ fWidth * fHeight * _elementSize];
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        memcpy( ptr, &data[((2*fHeight - j - 1)*_width + fWidth)*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }
    _data.push_back(face);

    // positive Z
    face = new GLubyte[ fWidth * fHeight * _elementSize];
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        memcpy( ptr, &data[((_height - (fHeight + j + 1))*_width + fWidth) * _elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }
    _data.push_back(face);

    // negative Z
    face = new GLubyte[ fWidth * fHeight * _elementSize];
    ptr = face;
    for (int j=0; j<fHeight; j++) {
        for (int i=0; i<fWidth; i++) {
            memcpy( ptr, &data[(j*_width + 2 * fWidth - (i + 1))*_elementSize], _elementSize);
            ptr += _elementSize;
        }
    }
    _data.push_back(face);

    //set the new # of faces, width and height
    _faces = 6;
    _width = fWidth;
    _height = fHeight;

    //delete the old pointer
    delete []data;

    return true;
}

bool Image::setImage( int width, int height, GLenum format, GLenum type, const void* data){
    //check parameters before destroying the old image
    int elementSize;
    GLenum internalFormat;

    switch (format) {
        case GL_ALPHA:
            switch (type) {
                case GL_UNSIGNED_BYTE:
                    internalFormat = GL_ALPHA8;
                    elementSize = 1;
                    break;
                case GL_UNSIGNED_SHORT:
                    internalFormat = GL_ALPHA16;
                    elementSize = 2;
                    break;
                case GL_FLOAT:
                    internalFormat = GL_ALPHA32F_ARB;
                    elementSize = 4;
                    break;
                case GL_HALF_FLOAT_ARB:
                    internalFormat = GL_ALPHA16F_ARB;
                    elementSize = 2;
                    break;
                default:
                    return false; //format/type combo not supported
            }
            break;
        case GL_LUMINANCE:
            switch (type) {
                case GL_UNSIGNED_BYTE:
                    internalFormat = GL_LUMINANCE8;
                    elementSize = 1;
                    break;
                case GL_UNSIGNED_SHORT:
                    internalFormat = GL_LUMINANCE16;
                    elementSize = 2;
                    break;
                case GL_FLOAT:
                    internalFormat = GL_LUMINANCE32F_ARB;
                    elementSize = 4;
                    break;
                case GL_HALF_FLOAT_ARB:
                    internalFormat = GL_LUMINANCE16F_ARB;
                    elementSize = 2;
                    break;
                default:
                    return false; //format/type combo not supported
            }
            break;
        case GL_LUMINANCE_ALPHA:
            switch (type) {
                case GL_UNSIGNED_BYTE:
                    internalFormat = GL_LUMINANCE8_ALPHA8;
                    elementSize = 2;
                    break;
                case GL_UNSIGNED_SHORT:
                    internalFormat = GL_LUMINANCE16_ALPHA16;
                    elementSize = 4;
                    break;
                case GL_FLOAT:
                    internalFormat = GL_LUMINANCE_ALPHA32F_ARB;
                    elementSize = 8;
                    break;
                case GL_HALF_FLOAT_ARB:
                    internalFormat = GL_LUMINANCE_ALPHA16F_ARB;
                    elementSize = 4;
                    break;
                default:
                    return false; //format/type combo not supported
            }
            break;
        case GL_RGB:
            switch (type) {
                case GL_UNSIGNED_BYTE:
                    internalFormat = GL_RGB8;
                    elementSize = 3;
                    break;
                case GL_UNSIGNED_SHORT:
                    internalFormat = GL_RGB16;
                    elementSize = 6;
                    break;
                case GL_FLOAT:
                    internalFormat = GL_RGB32F_ARB;
                    elementSize = 12;
                    break;
                case GL_HALF_FLOAT_ARB:
                    internalFormat = GL_RGB16F_ARB;
                    elementSize = 6;
                    break;
                default:
                    return false; //format/type combo not supported
            }
            break;
        case GL_RGBA:
            switch (type) {
                case GL_UNSIGNED_BYTE:
                    internalFormat = GL_RGBA8;
                    elementSize = 4;
                    break;
                case GL_UNSIGNED_SHORT:
                    internalFormat = GL_RGBA16;
                    elementSize = 8;
                    break;
                case GL_FLOAT:
                    internalFormat = GL_RGBA32F_ARB;
                    elementSize = 16;
                    break;
                case GL_HALF_FLOAT_ARB:
                    internalFormat = GL_RGBA16F_ARB;
                    elementSize = 8;
                    break;
                default:
                    return false; //format/type combo not supported
            }
            break;
        default:
            //bad format
            return false;
            break;
    }


    //clear old data
    freeData();

    GLubyte *newImage = new GLubyte[width*height*elementSize];
    memcpy( newImage, data, width*height*elementSize);

    _data.push_back(newImage);

    _width = width;
    _height = height;
    _elementSize = elementSize;
    _internalFormat = internalFormat;
    _levelCount = 1;
    _faces = 0;
    _depth = 0;
    _format = format;
    _type = type;

    return true;
}

void Image::flipSurface()
{
	unsigned int lineSize;

	int depth = (_depth) ? _depth : 1;

	if (!isCompressed()) {
		lineSize = _elementSize * _width;
		unsigned int sliceSize = lineSize * _height;

		GLubyte *tempBuf = new GLubyte[lineSize];

		for (int ii = 0; ii < depth; ii++) {
			GLubyte *top = _data[0] + ii*sliceSize;
			GLubyte *bottom = top + (sliceSize - lineSize);

			for (int jj = 0; jj < (_height >> 1); jj++) {
				memcpy(tempBuf, top, lineSize);
				memcpy(top, bottom, lineSize);
				memcpy(bottom, tempBuf, lineSize);

				top += lineSize;
				bottom -= lineSize;
			}
		}

		delete[]tempBuf;
	}
}