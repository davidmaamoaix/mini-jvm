#include "mini_jvm/utils/file_io.h"

#include <stdio.h>
#include <stdlib.h>

#include "mini_jvm/error/err_vm.h"

err_vm f_read_bytes(const char *filename, uint32_t *size, uint8_t **buffer) {
    err_vm ret_val = E_SUCC;

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return E_NOFD;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc(*size * sizeof(uint8_t));
    E_MEM_HANDLE(*buffer, ret_val, FREE_FILE);

    size_t read = fread(*buffer, 1, *size, file);
    if (read < *size) {
        ret_val = E_IOER;
        goto FREE_BUFFER;
    }

    goto FREE_FILE;

FREE_BUFFER:
    free(*buffer);
FREE_FILE:
    fclose(file);
END:
    return ret_val;
}
