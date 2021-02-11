#include "classloader.h"

#include <stdio.h>
#include <stdlib.h>

#include "util/debug.h"

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

    if (!file) {
        printf("[Error] The class file %s does not exist.\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    reader->end = ftell(file);
    rewind(file);

    reader->bytes = malloc((unsigned long) (reader->end + 1) * sizeof(char));
    fread(reader->bytes, sizeof(char), (unsigned long) reader->end, file);
    fclose(file);

    reader->bytes[reader->end] = '\0';
    return reader;
}

void loadClass(const char *path) {
    Reader *reader = readClass(path);

    if (readbytes_4(reader) != 0xCAFEBABE) {
        printf("[Error] The file %s is not a Java class file.", path);
        return;
    }

    VERBOSE("Loading Java class file: %s", path);


}