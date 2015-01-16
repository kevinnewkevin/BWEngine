#include "png.h"
#include "Image.h"
#include <stdio.h>

static void pngReadFn( png_structp png_ptr, png_bytep data, png_size_t length) {
    FILE* fp = (FILE*)png_get_io_ptr(png_ptr);

    if (!data)
        png_error( png_ptr, "Attempt to read from null file pointer");

    fread( data, length, 1, fp);
}

static void pngWriteFn( png_structp png_ptr, png_bytep data, png_size_t length) {
    FILE* fp = (FILE*)png_get_io_ptr(png_ptr);

    if (!data)
        png_error( png_ptr, "Attempt to write to null file pointer");

    fwrite( data, length, 1, fp);
}

static void pngFlushFn( png_structp png_ptr) {
    FILE* fp = (FILE*)png_get_io_ptr(png_ptr);

    fflush(fp);
}

bool loadImagePNG( const char *file, Image& i) 
{
    FILE *fp = fopen( file, "rb");

    if ( !fp)
        return false;

    GLubyte signature[8];
    fread( signature, 8, 1, fp);

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    if (!png_check_sig( signature, 8)) {
        fclose(fp);
        return false;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return false;   /* out of memory */
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return false;   /* out of memory */
    }

    // setjmp() is used for error handling with libPNG, if something goes wrong it is coming back here

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }

    // Need to override the standard I/O methods since libPNG may be linked against a different run-time
    png_set_read_fn( png_ptr, fp, pngReadFn);

    png_set_sig_bytes(png_ptr, 8);  // skip the sig bytes

    png_read_info(png_ptr, info_ptr);  // automagically read everything to the image data

    i._width = png_get_image_width(png_ptr, info_ptr);
    i._height = png_get_image_height(png_ptr, info_ptr);
    i._depth = 0; // using the convention of depth == 0 for 2D images
    int colorType = png_get_color_type( png_ptr, info_ptr);
    int bitDepth = png_get_bit_depth( png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) { 
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return false;
    }

    //Setup the read transforms
    // expand palette images to RGB and low-bit-depth grayscale images to 8 bits
    // convert transparency chunks to full alpha channel
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    // now configure for reading, and allocate the memory
    png_read_update_info(png_ptr, info_ptr);

    int rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    GLubyte *data = new GLubyte[rowBytes * i._height];

    GLubyte **rowPointers = new GLubyte*[i._height]; 

    // set up the row pointers
    for ( int ii = 0;  ii < i._height; ii++) {
        rowPointers[ii] = data + ii*rowBytes;
    }

    // read the image
    png_read_image(png_ptr, rowPointers);


    // reading is complete, configure other parameters

    delete []rowPointers;
    bool use16 = bitDepth > 8;

    i._type = (use16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;

    switch ((int)png_get_channels(png_ptr, info_ptr)) {
        case 1:
            i._format = GL_LUMINANCE;
            i._internalFormat = (use16) ? GL_LUMINANCE16 : GL_LUMINANCE8;
            i._elementSize = (use16) ? 2 : 1;
            break;
        case 2:
            i._format = GL_LUMINANCE_ALPHA;
            i._internalFormat = (use16) ? GL_LUMINANCE16_ALPHA16 : GL_LUMINANCE8_ALPHA8;
            i._elementSize = (use16) ? 4 : 2;
            break;
        case 3:
            i._format = GL_RGB;
            i._internalFormat = (use16) ? GL_RGB16 : GL_RGB8;
            i._elementSize = (use16) ? 6 : 3;
            break;
        case 4:
            i._format = GL_RGBA;
            i._internalFormat = (use16) ? GL_RGBA16 : GL_RGBA8;
            i._elementSize = (use16) ? 8 : 4;
            break;
    }

    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    //finalize parameters
    i._data.push_back( data);
    i._levelCount = 1;
    i._faces = 0;
    i._depth = 0;
	i.flipSurface();
    
    return true;
}