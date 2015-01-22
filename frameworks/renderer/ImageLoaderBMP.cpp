#include "Image.h"
#include <stdio.h>
#include <windows.h>
#include "ImageLoader.h"

bool loadImageBMP(const char *file, Image& i)
{
	FILE *fp; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
	BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    fp = fopen(file, "rb");
    if (fp == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,fp);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(fp);
        return NULL;
    }

    //read the bitmap info header
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,fp); // small edit. forgot to add the closing bracket at sizeof

	bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth*bitmapInfoHeader.biHeight*(bitmapInfoHeader.biBitCount>>3);
    //move file point to the begging of bitmap data
    fseek(fp, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader.biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(fp);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.biSizeImage, 1, fp);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(fp);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
	for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

	i._width = bitmapInfoHeader.biWidth;
	i._height = bitmapInfoHeader.biHeight;
	i._levelCount = 1;
	i._format = GL_RGB;
	i._data.push_back(bitmapImage);
    //close file and return bitmap iamge data
    fclose(fp);
    return true;
}

