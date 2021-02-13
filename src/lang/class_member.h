#ifndef MINIJVM_CLASS_MEMBER_H
#define MINIJVM_CLASS_MEMBER_H

#include <inttypes.h>

// an interface just points to an entry in the
// constant pool
typedef uint16_t Interface;

typedef struct Attribute {

    // index to an UTF-8 entry in the constant pool
    uint16_t attrIndex;

    // attributes
    uint32_t attrLen;
    uint8_t *info;
} Attribute;

typedef struct Field {
    uint16_t accessFlag;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t attrCount;
    Attribute *attrs;
} Field;

typedef struct Method {
    
} Method;

#endif //MINIJVM_CLASS_MEMBER_H
