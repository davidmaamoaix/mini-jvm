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

// denotes the blocks of exception handlers
typedef struct ExceptionBlock {
    uint16_t startPos;
    uint16_t endPos;
    uint16_t handlerPos;
    uint16_t catchType;
} ExceptionBlock;

typedef struct CodeAttr {
    uint16_t maxStack;
    uint16_t maxLocals;
    uint32_t codeLen;
    uint8_t *code;
    uint16_t excTableLen;
    ExceptionBlock *excTable;
    uint16_t attrCount;
    Attribute *attrs;
} CodeAttr;

// the struct is identical to Field, but distinguishing
// them in case of additional data that are calculated
// during read-time, or just space for debugging in general
typedef struct Method {
    uint16_t accessFlag;
    uint16_t nameIndex;
    uint16_t descriptorIndex;
    uint16_t attrCount;
    Attribute *attrs;

    CodeAttr codeAttr;
} Method;

#endif //MINIJVM_CLASS_MEMBER_H
