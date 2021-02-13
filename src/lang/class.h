#ifndef MINIJVM_CLASS_H
#define MINIJVM_CLASS_H

#include <inttypes.h>

#include "lang/class_member.h"
#include "lang/internal/const_pool.h"

typedef struct Class {

    // version of the class format
    uint16_t major;
    uint16_t minor;

    // constant pool
    uint16_t constPoolSize;
    Constant *constPool;

    // access flags (a mask specifying the class modifiers)
    uint16_t accessFlags;

    // index to classes in the constant pool
    uint16_t thisClass;
    uint16_t superClass; // 0 in case of the Object class

    // interfaces
    uint16_t interfaceCount;
    Interface *interfaces;

    // fields
    uint16_t fieldCount;
    Field *fields;

    // methods
    uint16_t methodCount;
    Method *methods;
} Class;

#endif //MINIJVM_CLASS_H
