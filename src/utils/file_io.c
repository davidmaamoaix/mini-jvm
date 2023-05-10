#include "mini_jvm/utils/file_io.h"

#include <stdio.h>
#include <stdlib.h>

// TODO: refractor this shit pile
int64_t f_read_bytes(const char *filename, uint8_t **buffer) {
    int64_t size;
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        size = -1;
        goto END;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc(size);
    if (buffer == NULL) {
        printf("Malloc failed");
        size = -1;
        goto FREE_FILE;
    }

    size_t read = fread(*buffer, 1, size, file);
    if (read < size) {
        size = -1;
        goto FREE_BUFFER;
    }

    goto FREE_FILE;

FREE_BUFFER:
    free(*buffer);
FREE_FILE:
    fclose(file);
END:
    return size;
}
