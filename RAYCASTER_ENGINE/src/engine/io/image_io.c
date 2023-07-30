#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "image_io.h"
#include "..\util.h"

#define IO_IMAGE_READ_ERROR_MESSAGE "\nFailed to load image:\nat line %d in file:\n%s"
#define IO_IMAGE_FREE_ERROR_MESSAGE "\nImage data passed was NULL:\nat line %d in file:\n%s"

ImageFile io_image_read(const char* path, bool flip) {
	ImageFile image = { .is_valid = false };

	stbi_set_flip_vertically_on_load(flip);

	image.image_data = stbi_load(path, &image.width, &image.height, &image.channels, 0);
	if (image.image_data == NULL)
		ERROR_RETURN(image, IO_IMAGE_READ_ERROR_MESSAGE, __LINE__, __FILE__);

	image.is_valid = true;

	return image;
}

void io_image_free(unsigned char* image_data) {
	if (!image_data)
		ERROR_RETURN(, IO_IMAGE_FREE_ERROR_MESSAGE, __LINE__, __FILE__);

	stbi_image_free(image_data);
}