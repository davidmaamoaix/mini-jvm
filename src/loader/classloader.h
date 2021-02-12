#ifndef MINIJVM_CLASSLOADER_H
#define MINIJVM_CLASSLOADER_H

#include <inttypes.h>

#include "lang/class.h"

typedef struct Reader {
    unsigned char *bytes;
    long reg;
    long end;
    long error;
} Reader;

uint8_t readbytes_1(Reader *);
uint16_t readbytes_2(Reader *);
uint32_t readbytes_4(Reader *);
uint64_t readbytes_8(Reader *);
const unsigned char *readBytes(Reader *, uint16_t);

Reader *readClass(const char *);
Class *loadClass(const char *);

Constant *readConstPool(Reader *, uint16_t);

#endif //MINIJVM_CLASSLOADER_H
