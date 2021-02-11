#ifndef MINIJVM_CLASS_H
#define MINIJVM_CLASS_H

#include <inttypes.h>

typedef struct Class {
    uint16_t major;
    uint16_t minor;
} Class;

#endif //MINIJVM_CLASS_H
