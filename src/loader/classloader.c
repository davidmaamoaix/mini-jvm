#include "classloader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/debug.h"

ByteStream *readClass(const char *path) {
    FILE *file = fopen(path, "rb");

    if (!file) {
        ERROR("[Error] The class file %s does not exist.\n", path);
        exit(2);
    }

    ByteStream *reader = malloc(sizeof(ByteStream));

    if (reader == NULL) {
        ERROR("[Error] Malloc failed during initializing class reader\n");
        exit(12);
    }

    reader->reg = 0;

    fseek(file, 0, SEEK_END);
    reader->end = (uint32_t) ftell(file);
    rewind(file);

    reader->bytes = malloc((unsigned long) (reader->end + 1) * sizeof(char));
    fread(reader->bytes, sizeof(char), (unsigned long) reader->end, file);
    fclose(file);

    reader->bytes[reader->end] = '\0';
    return reader;
}

Class *loadClass(const char *path) {
    ByteStream *reader = readClass(path);

    if (readbytes_4(reader) != 0xCAFEBABE) {
        ERROR("[Error] The file %s is not a Java class file\n", path);
        exit(1);
    }

    VERBOSE("Loading Java class file: %s\n", path);

    Class *class = malloc(sizeof(Class));

    if (class == NULL) {
        ERROR("[Error] Malloc failed during class loading\n");
        exit(12);
    }

    // class format version

    class->minor = readbytes_2(reader);
    class->major = readbytes_2(reader);

    VERBOSE("Class file format version: %d %d\n", class->major, class->minor);

    // constant pool

    class->constPoolSize = readbytes_2(reader);

    VERBOSE("Reading constant pool of size %d\n", class->constPoolSize);

    class->constPool = readConstPool(reader, class->constPoolSize);

    // access flags

    class->accessFlags = readbytes_2(reader);

    // class hierarchy
    class->thisClass = readbytes_2(reader);
    class->superClass = readbytes_2(reader);

    // interfaces

    class->interfaceCount = readbytes_2(reader);

    VERBOSE("Reading %d interfaces\n", class->interfaceCount);

    class->interfaces = readInterfaces(reader, class->interfaceCount);

    // fields

    class->fieldCount = readbytes_2(reader);

    VERBOSE("Reading %d fields\n", class->fieldCount);

    class->fields = readFields(reader, class->fieldCount);

    // methods

    class->methodCount = readbytes_2(reader);

    VERBOSE("Reading %d methods\n", class->methodCount);

    class->methods = readMethods(reader, class->methodCount);

    class->attrCount = readbytes_2(reader);

    VERBOSE("Reading %d attributes\n", class->attrCount);

    class->attrs = readAttrs(reader, class->attrCount);

    // the error is specified during the reader->err = 1
    /*if (reader->error) {
        ERROR("Loading class %s failed\n", path);

        exit(1);
    }*/

    parseMethodCode(class);

    if (reader->reg != reader->end) {
        ERROR(
            "Class ended at %d while the file is %d long\n",
            reader->reg,
            reader->end
        );
        exit(1);
    }

    free(reader->bytes);
    free(reader);

    return class;
}

void freeClass(Class *class) {
    freeConstPool(class->constPool, class->constPoolSize);
    freeFields(class->fields, class->fieldCount);
    freeMethods(class->methods, class->methodCount);

    free(class);
}

Constant *readConstPool(ByteStream *reader, uint16_t size) {

    // constant pool index starts at 1
    Constant *constPool = malloc((size + 1) * sizeof(Constant));

    if (constPool == NULL) {
        ERROR("[Error] Malloc failed for constant pool\n");
        exit(12);
    }

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
                ERROR(
                    "[Error] Unexpected constant tag: %d\n",
                    constPool[i].tag
                );

                exit(1);
        }
    }

    return constPool;
}

void freeConstPool(Constant *constPool, uint16_t size) {

    if (constPool == NULL) return;

    // free all allocated strings in the const table
    for (uint16_t i = 1; i < size; ++i) {
        if (constPool[i].tag == CONSTANT_Utf8) {
            free(constPool[i].string);
        }
    }

    free(constPool);
}

Interface *readInterfaces(ByteStream *reader, uint16_t size) {

    Interface *interfaces = malloc(size * sizeof(Interface));

    if (interfaces == NULL) {
        ERROR("[Error] Malloc failed during reading interfaces\n");
        exit(12);
    }

    // unlike the constant pool, the interface pool
    // starts at index 0
    for (uint16_t i = 0; i < size; ++i) {
        interfaces[i] = readbytes_2(reader);
    }

    return interfaces;
}

Field *readFields(ByteStream *reader, uint16_t size) {

    Field *fields = malloc(size * sizeof(Field));

    if (fields == NULL) {
        ERROR("[Error] Malloc failed during reading fields\n");
        exit(12);
    }

    for (uint16_t i = 0; i < size; ++i) {
        fields[i].accessFlag = readbytes_2(reader);
        fields[i].nameIndex = readbytes_2(reader);
        fields[i].descriptorIndex = readbytes_2(reader);
        fields[i].attrCount = readbytes_2(reader);

        fields->attrs = readAttrs(reader, fields[i].attrCount);
    }

    return fields;
}

void freeFields(Field *fields, uint16_t size) {

    if (fields == NULL) return;

    for (uint16_t i = 0; i < size; ++i) {
        freeAttrs(fields[i].attrs, fields[i].attrCount);
    }

    free(fields);
}

Attribute *readAttrs(ByteStream *reader, uint16_t size) {

    Attribute *attrs = malloc(size * sizeof(Attribute));

    if (attrs == NULL) {
        ERROR("[Error] Malloc failed during reading attributes\n");
        exit(12);
    }

    for (uint16_t i = 0; i < size; ++i) {
        attrs[i].attrIndex = readbytes_2(reader);
        attrs[i].attrLen = readbytes_4(reader);

        attrs[i].info = malloc(attrs[i].attrLen * sizeof(uint8_t));

        if (attrs[i].info == NULL) {
            ERROR("[Error] Malloc failed during attribute info\n");

            exit(12);
        }

        for (uint16_t byteId = 0; byteId < attrs[i].attrLen; ++byteId) {
            attrs[i].info[byteId] = readbytes_1(reader);
        }
    }

    return attrs;
}

void freeAttrs(Attribute *attrs, uint16_t size) {

    if (attrs == NULL) return;

    for (uint16_t i = 0; i < size; ++i) {
        free(attrs[i].info);
    }

    free(attrs);
}

Method *readMethods(ByteStream *reader, uint16_t size) {

    Method *methods = malloc(size * sizeof(Method));

    if (methods == NULL) {
        ERROR("[Error] Malloc failed during initializing methods\n");
        exit(12);
    }

    for (uint16_t i = 0; i < size; ++i) {
        methods[i].accessFlag = readbytes_2(reader);
        methods[i].nameIndex = readbytes_2(reader);
        methods[i].descriptorIndex = readbytes_2(reader);
        methods[i].attrCount = readbytes_2(reader);

        methods[i].attrs = readAttrs(reader, methods[i].attrCount);
    }

    return methods;
}

void freeMethods(Method *methods, uint16_t size) {

    if (methods == NULL) return;

    for (uint16_t i = 0; i < size; ++i) {
        freeAttrs(methods[i].attrs, methods[i].attrCount);
    }

    free(methods);
}

uint8_t verifyClass(Class *class) {

    // field attrs
    for (uint16_t i = 0; i < class->fieldCount; ++i) {
        // TODO: do this stuff
    }

    return 1;
}

void parseMethodCode(Class *class) {

    for (uint16_t i = 0; i < class->methodCount; ++i) {
        Method *method = &class->methods[i];

        for (uint16_t attr = 0; attr < method->attrCount; ++attr) {
            uint16_t attrIndex = method->attrs[attr].attrIndex;
            Constant *tag = &class->constPool[attrIndex];

            if (tag->tag == CONSTANT_Utf8 &&
                strcmp((const char *) tag->string, "Code") == 0) {

                unsigned char *methodName = class->constPool[method->nameIndex].string;
                VERBOSE(
                    "\nLoading code of length %d for method %s:\n",
                    method->attrs[attr].attrLen,
                    methodName
                );

                ByteStream *reader = malloc(sizeof(ByteStream));

                if (reader == NULL) {
                    ERROR("[Error] Malloc failed during initializing code\n");
                    exit(12);
                }

                reader->reg = 0;
                reader->end = method->attrs[attr].attrLen;
                reader->bytes = method->attrs[attr].info;

                CodeAttr *code = &method->codeAttr;

                code->maxStack = readbytes_2(reader);
                code->maxLocals = readbytes_2(reader);
                code->codeLen = readbytes_4(reader);

                VERBOSE(
                    "Stack size: %d\nLocal size: %d\nBytecode length: %d\n",
                    code->maxStack,
                    code->maxLocals,
                    code->codeLen
                );

                code->code = malloc(code->codeLen * sizeof(uint8_t));

                if (code->code == NULL) {
                    ERROR("[Error] Malloc failed during initializing code container\n");
                    exit(12);
                }

                for (uint16_t j = 0; j < code->codeLen; ++j) {
                    code->code[j] = readbytes_1(reader);
                }

                code->excTableLen = readbytes_2(reader);

                VERBOSE("Exception handler count: %d\n", code->excTableLen);

                code->excTable = malloc(code->excTableLen * sizeof(ExceptionBlock));

                if (code->excTable == NULL) {
                    ERROR("[Error] Malloc failed during initializing code exc\n");
                    exit(12);
                }

                for (uint16_t j = 0; j < code->excTableLen; ++j) {
                    code->excTable[j].startPos = readbytes_2(reader);
                    code->excTable[j].endPos = readbytes_2(reader);
                    code->excTable[j].handlerPos = readbytes_2(reader);
                    code->excTable[j].catchType = readbytes_2(reader);
                }

                code->attrCount = readbytes_2(reader);
                code->attrs = readAttrs(reader, code->attrCount);

                if (reader->reg != reader->end) {
                    ERROR("[Error] Code longer than expected at %s\n", methodName);
                }

                free(reader);
            }
        }
    }
}