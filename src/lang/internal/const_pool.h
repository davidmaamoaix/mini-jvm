#ifndef MINIJVM_CONST_POOL_H
#define MINIJVM_CONST_POOL_H

#include <inttypes.h>

// Naming convention according to
// https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4

#define CONSTANT_Utf8 1
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_Long 5
#define CONSTANT_Double 6
#define CONSTANT_Class 7
#define CONSTANT_String 8
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType 12
#define CONSTANT_MethodHandle 15
#define CONSTANT_MethodType 16
#define CONSTANT_InvokeDynamic 18

typedef struct Constant {
    unsigned char tag;

    // string is NULL unless the internal is CONSTANT_Utf8, in
    // which case value is ignored
    uint32_t value;

    uint16_t strLen;
    unsigned char *string;
} Constant;

#endif //MINIJVM_CONST_POOL_H
