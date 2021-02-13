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

    Reader *reader = malloc(sizeof(Reader));
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

    class->constPool = readConstPool(reader, class->constPoolSize);

    // the error is specified during the reader->err = 1
    if (reader->error) {
        printf("Loading class %s failed\n", path);

        free(class);

        return NULL;
    }

    return class;
}

Constant *readConstPool(Reader *reader, uint16_t size) {

    if (reader->error) return NULL;

    // constant pool index starts at 1
    Constant *constPool = malloc((size + 1) * sizeof(Constant));

    for (int i = 1; i < size; ++i) {
        constPool[i].tag = readbytes_1(reader);

        switch (constPool[i].tag) {
            case CONSTANT_Utf8:
                constPool[i].strLen = readbytes_2(reader);
                constPool[i].string = readBytes(reader, constPool[i].strLen);
                break;
            case CONSTANT_Integer:
            case CONSTANT_Float:
                constPool[i].value = readbytes_4(reader);
                break;
            case CONSTANT_Long:
            case CONSTANT_Double:
                // long and double takes up 2 entries
                constPool[i].value = readbytes_4(reader);
                constPool[++i].value = readbytes_4(reader);
                break;
            case CONSTANT_Class:
            case CONSTANT_String:
            case CONSTANT_MethodType:
                // all are treated similarly:
                // class & str: index of UTF-8
                // methodType: descriptor index
                constPool[i].value = readbytes_2(reader);
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_NameAndType:
            case CONSTANT_InvokeDynamic:
                // abuse the 4 bytes int to store 2 2 bytes int,
                // corresponding to name_index and descriptor/type index

                // in the case of invokeDynamic, 2 bytes are dedicated to
                // an index in the bootstrap method table, and 2 bytes to
                // an index to nameAndType in the constant poolg
                constPool[i].value = (uint32_t) readbytes_2(reader) << 16;
                constPool[i].value |= readbytes_2(reader);
                break;
            case CONSTANT_MethodHandle:
                constPool[i].value = (uint32_t) readbytes_1(reader) << 16;
                constPool[i].value |= readbytes_2(reader);
                break;
            default:
                reader->error = 1;
                printf("Unexpected constant tag: %d", constPool[i].tag);
                break;
        }
    }

    return constPool;
}