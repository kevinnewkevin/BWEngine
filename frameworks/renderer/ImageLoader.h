#pragma once
#include "Image.h"

bool loadImagePNG(const char *file, Image& i);

bool loadImageJPG(const char *file, Image& i);

bool loadImageTGA(const char *file, Image& i);