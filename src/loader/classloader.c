#include "classloader.h"

#include <stdio.h>
#include <stdlib.h>

#include "util/debug.h"

uint8_t readbytes_1(Reader *reader) {
    if (reader->reg == reader->end) {
        printf("[ERROR] End of file reached.");
        reader->error = 1;
        return 0;
    }

    if (reader->error) return 0;

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

const unsigned char *readBytes(Reader *reader, uint16_t size) {
    unsigned char *bytes = malloc(size * sizeof(unsigned char));

    for (uint16_t i = 0; i < size; ++i) {
        bytes[i] = readbytes_1(reader);
    }

    return bytes;
}

Reader *readClass(const char *path) {
    FILE *file = fopen(path, "rb");

    if (!file) {
        printf("[Error] The class file %s does not exist.\n", path);
        return NULL;
    }

    Reader *reader = malloc(sizeof(reader));
    reader->reg = 0;
    reader->error = 0;

    fseek(file, 0, SEEK_END);
    reader->end = ftell(file);
    rewind(file);

    reader->bytes = malloc((unsigned long) (reader->end + 1) * sizeof(char));
    fread(reader->bytes, sizeof(char), (unsigned long) reader->end, file);
    fclose(file);

    reader->bytes[reader->end] = '\0';
    return reader;
}

Class *loadClass(const char *path) {
    Reader *reader = readClass(path);

    if (readbytes_4(reader) != 0xCAFEBABE) {
        printf("[Error] The file %s is not a Java class file.", path);
        return NULL;
    }

    VERBOSE("Loading Java class file: %s\n", path);

    Class *class = malloc(sizeof(Class));
    class->minor = readbytes_2(reader);
    class->major = readbytes_2(reader);

    VERBOSE("Class file format version: %d %d\n", class->major, class->minor);

    class->constPoolSize = readbytes_2(reader);

    VERBOSE("Reading constant pool of size %d\n", class->constPoolSize);

    class->constPool = readConstPool(class->constPoolSize, reader);
}

Constant *readConstPool(uint16_t size, Reader *reader) {

    // constant pool index starts at 1
    Constant *constPool = malloc((size + 1) * sizeof(Constant));

    for (int i = 1; i < size; ++i) {
        constPool[i].tag = readbytes_1(reader);

        
    }
}