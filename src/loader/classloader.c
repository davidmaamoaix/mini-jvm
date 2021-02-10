#include "classloader.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t readbytes_1(Reader *reader) {
    return reader->bytes[reader->reg++];
}

uint16_t readbytes_2(Reader *reader) {
    return (uint16_t) readbytes_1(reader) << 8 | readbytes_1(reader);
}

uint32_t readbytes_4(Reader *reader) {
    return (uint32_t) readbytes_2(reader) << 16 | readbytes_2(reader);
}

uint64_t readbytes_8(Reader *reader) {
    return (uint64_t) readbytes_4(reader) << 32 | readbytes_4(reader);
}

Reader *readClass(const char *path) {
    Reader *reader = malloc(sizeof(reader));
    reader->reg = 0;

    FILE *file = fopen(path, "rb");

    return reader;
}