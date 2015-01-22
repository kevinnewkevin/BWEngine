#include "png.h"
#include "Image.h"
#include <stdio.h>
#include "ImageLoader.h"
#include "utils/FileUtils.h"

static void pngReadFn( png_structp png_ptr, png_bytep data, png_size_t length) {
	Data* fp = (Data*)png_get_io_ptr(png_ptr);

    if (!data)
        png_error( png_ptr, "Attempt to read from null file pointer");
	fp->readBytes(data, length);
}

bool loadImagePNG( const char *file, Image& i) 
{
	Data dataSource = FileUtils::getInstance()->getDataFromFile(file);

    GLubyte signature[8];
	dataSource.readBytes(signature, 8);

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    if (!png_check_sig( signature, 8)) {
        return false;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        return false;   /* out of memory */
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;   /* out of memory */
    }

    // setjmp() is used for error handling with libPNG, if something goes wrong it is coming back here

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    // Need to override the standard I/O methods since libPNG may be linked against a different run-time
    png_set_read_fn( png_ptr, &dataSource, pngReadFn);

    png_set_sig_bytes(png_ptr, 8);  // skip the sig bytes

    png_read_info(png_ptr, info_ptr);  // automagically read everything to the image data

    i._width = png_get_image_width(png_ptr, info_ptr);
    i._height = png_get_image_height(png_ptr, info_ptr);
    i._depth = 0; // using the convention of depth == 0 for 2D images
    int colorType = png_get_color_type( png_ptr, info_ptr);
    int bitDepth = png_get_bit_depth( png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) { 
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
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
    for (int ii = i._height - 1; ii >= 0; ii--) {
		rowPointers[ii] = data + (i._height - ii - 1)*rowBytes;
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

    //finalize parameters
    i._data.push_back( data);
    i._levelCount = 1;
    i._faces = 0;
    i._depth = 0;
    return true;
}

// -------------------------------------------------------------------------------
#include "jpeglib.h" 
struct loader_jpeg_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

void init_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}

boolean fill_input_buffer (j_decompress_ptr cinfo)
{
	// DO NOTHING
	return 1;
}

void skip_input_data (j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr * src = cinfo->src;
	if(count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}

void term_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}

void error_exit(j_common_ptr cinfo)
{
}

void output_message(j_common_ptr cinfo)
{
}

bool loadImageJPG(const char *file, Image& i)
{
	FILE* fp = fopen(file, "rb");
	if (fp == NULL)
		return false;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	unsigned char* buff = new unsigned char[size];
	fseek(fp, 0, SEEK_SET);
	fread(buff, 1, size, fp);

		// allocate and initialize JPEG decompression object
	jpeg_decompress_struct cinfo;
	loader_jpeg_error_mgr jerr;
	unsigned char** rowPtr;

	//We have to set up the error handler first, in case the initialization
	//step fails.  (Unlikely, but it could happen if you are out of memory.)
	//This routine fills in the contents of struct jerr, and returns jerr's
	//address which we place into the link field in cinfo.

	cinfo.err = jpeg_std_error(&jerr.pub);
	cinfo.err->error_exit = error_exit;
	cinfo.err->output_message = output_message;

	// compatibility fudge:
	// we need to use setjmp/longjmp for error handling as gcc-linux
	// crashes when throwing within external c code
	if (setjmp(jerr.setjmp_buffer))
	{
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object and return.

		jpeg_destroy_decompress(&cinfo);

		delete [] buff;
		// if the row pointer was created, we delete it.
		if (rowPtr)
			delete [] rowPtr;

		// return null pointer
		return false;
	}

	// Now we can initialize the JPEG decompression object.
	jpeg_create_decompress(&cinfo);

	// specify data source
	jpeg_source_mgr jsrc;

	// Set up data pointer
	jsrc.bytes_in_buffer = size;
	jsrc.next_input_byte = (JOCTET*)buff;
	cinfo.src = &jsrc;

	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_input_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = term_source;

	jpeg_read_header(&cinfo, TRUE);

	bool useCMYK=false;
	if (cinfo.jpeg_color_space==JCS_CMYK)
	{
		cinfo.out_color_space=JCS_CMYK;
		cinfo.out_color_components=4;
		useCMYK=true;
	}
	else
	{
		cinfo.out_color_space=JCS_RGB;
		cinfo.out_color_components=3;
		i._internalFormat = GL_RGB;
		i._elementSize = 3;
		i._format = GL_RGB;
	}
	cinfo.output_gamma=2.2;
	cinfo.do_fancy_upsampling=FALSE;

	jpeg_start_decompress(&cinfo);

	unsigned short rowspan = cinfo.image_width * cinfo.out_color_components;
	unsigned int width = cinfo.image_width;
	unsigned int height = cinfo.image_height;

	unsigned char* output = new unsigned char[rowspan * height];

	// Here we use the library's state variable cinfo.output_scanline as the
	// loop counter, so that we don't have to keep track ourselves.
	// Create array of row pointers for lib
	rowPtr = new unsigned char* [height];

	for( unsigned int ii = 0; ii < height; ii++ )
		rowPtr[ii] = &output[ ii * rowspan ];

	unsigned int rowsRead = 0;

	while( cinfo.output_scanline < cinfo.output_height )
		rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

	delete [] rowPtr;
	// Finish decompression

	jpeg_finish_decompress(&cinfo);

	// Release JPEG decompression object
	// This is an important step since it will release a good deal of memory.
	jpeg_destroy_decompress(&cinfo);

	i._width = width;
	i._height = height;
	unsigned char* imageData;
	// convert image
	if (useCMYK)
	{
		imageData = new unsigned char[3 * width*height];
		if (imageData)
		{
			for (int ii = 0, j = 0; ii < size; ii += 3, j += 4)
			{
				// Also works without K, but has more contrast with K multiplied in
				//				data[i+0] = output[j+2];
				//				data[i+1] = output[j+1];
				//				data[i+2] = output[j+0];
				imageData[ii + 0] = (char)(output[j + 2] * (output[j + 3] / 255.f));
				imageData[ii + 1] = (char)(output[j + 1] * (output[j + 3] / 255.f));
				imageData[ii + 2] = (char)(output[j + 0] * (output[j + 3] / 255.f));
			}
		}
		delete[] output;
	}
	else
		imageData = output;

	i._data.push_back(imageData);
    i._levelCount = 1;
    i._faces = 0;
    i._depth = 0;
	i.flipSurface();
	return true;
}


// TGA
#include "extern/libtga/TGAlib.h"
bool loadImageTGA(const char *file, Image& i)
{
	bool ret = false;
	auto info = tgaLoad(file);
	if (info != nullptr && info->status == TGA_OK)
	{
		do
		{
			BREAK_IF(info == nullptr);
			// tgaLoadBuffer only support type 2, 3, 10
			if (2 == info->type || 10 == info->type)
			{
				// true color
				// unsupport RGB555
				if (info->pixelDepth == 16)
				{
					i._internalFormat = GL_RGB5_A1;
					i._format = GL_RGB5_A1;
					ret = true;
				}
				else if (info->pixelDepth == 24)
				{
					i._internalFormat = GL_RGB;
					i._format = GL_RGB;
					ret = true;
				}
				else if (info->pixelDepth == 32)
				{
					i._internalFormat = GL_RGBA;
					i._format = GL_RGBA;
					ret = true;
				}
				else
				{
					LOG("Image WARNING: unsupport true color tga data pixel format. FILE: %s", file);
					break;
				}
			}

		} while (false);

		if (ret)
		{
			i._width = info->width;
			i._height = info->height;
			i._data.push_back(info->imageData);
			i._levelCount = 1;
			i._faces = 0;
			i._depth = info->pixelDepth;
		}
		else
		{
			if (info && info->imageData != nullptr)
			{
				free(info->imageData);
			}
		}
	}
	SAFE_FREE(info);
	return ret;
}