#pragma once
#include <stdbool.h>
#include "..\types.h"

typedef struct {
    char* file_data;
    usize length;
    bool is_valid;
} File;

File io_file_read(const char* path);
void io_file_write(const char* path, const char* buffer, bool append);