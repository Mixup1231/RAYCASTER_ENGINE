#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "file_io.h"
#include "..\util.h"

#define IO_FILE_ERROR_MESSAGE_OPEN       "\nFailed to open file:\n%s\nat line %d in project file:\n%s\n"
#define IO_FILE_ERROR_MESSAGE_ALLOCATION "\nFailed allocating memory to read file:\n%s\nat line %d in project file:\n%s\n"
#define IO_FILE_ERROR_MESSAGE_NULL       "\nPointer passed was NULL:\nat %d in file:\n%s"

File io_file_read(const char* path) {
    File file = { .is_valid = false };

    if (!path)
        ERROR_RETURN(file, IO_FILE_ERROR_MESSAGE_OPEN, path, __LINE__, __FILE__);

    FILE* fp = NULL;
    fopen_s(&fp, path, "rb");
    if (!fp || ferror(fp))
        ERROR_RETURN(file, IO_FILE_ERROR_MESSAGE_OPEN, path, __LINE__, __FILE__);

    struct stat st;
    fstat(_fileno(fp), &st);

    file.length = st.st_size + 1;
    file.file_data = malloc(file.length);
    if (file.file_data == NULL) {
        fclose(fp);
        ERROR_RETURN(file, IO_FILE_ERROR_MESSAGE_ALLOCATION, path, __LINE__, __FILE__);
    }
    fread_s(file.file_data, st.st_size, st.st_size, 1, fp);
    fclose(fp);

    file.file_data[st.st_size] = 0;
    file.is_valid = true;

    return file;
}

void io_file_write(const char* path, const char* buffer, bool append) {
    if (!buffer || !path)
        ERROR_RETURN(, IO_FILE_ERROR_MESSAGE_NULL, __LINE__, __FILE__);

    FILE* fp = NULL;
    if (append) fopen_s(&fp, path, "ab");
    else fopen_s(&fp, path, "wb");
    if (!fp || ferror(fp))
        ERROR_RETURN(, IO_FILE_ERROR_MESSAGE_OPEN, path, __LINE__, __FILE__);

    fprintf_s(fp, buffer);
    fclose(fp);
}