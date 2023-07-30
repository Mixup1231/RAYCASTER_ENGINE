#pragma once
#include <stdbool.h>

#include "..\types.h"

typedef struct {
	u8* image_data;
	int width, height, channels;
	bool is_valid;
} ImageFile;

ImageFile io_image_read(const char* path, bool flip);
void io_image_free(unsigned char* image_data);