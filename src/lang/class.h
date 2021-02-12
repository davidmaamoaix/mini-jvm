#ifndef MINIJVM_CLASS_H
#define MINIJVM_CLASS_H

#include <inttypes.h>

#include "lang/internal/const_pool.h"

typedef struct Class {

    // version of the class format
    uint16_t major;
    uint16_t minor;

    // constant pool
    uint16_t constPoolSize;
    Constant *constPool;
} Class;

#endif //MINIJVM_CLASS_H
