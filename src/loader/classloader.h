#ifndef MINIJVM_CLASSLOADER_H
#define MINIJVM_CLASSLOADER_H

#include <inttypes.h>

#include "lang/class.h"

typedef struct Reader {
    unsigned char *bytes;
    long reg;
    long end;
    int error;
} Reader;

uint8_t readbytes_1(Reader *);
uint16_t readbytes_2(Reader *);
uint32_t readbytes_4(Reader *);
uint64_t readbytes_8(Reader *);

Reader *readClass(const char *);
Class *loadClass(const char *);

#endif //MINIJVM_CLASSLOADER_H
