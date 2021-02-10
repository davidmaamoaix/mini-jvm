#ifndef MINIJVM_CLASSLOADER_H
#define MINIJVM_CLASSLOADER_H

#include <inttypes.h>

typedef struct Reader {
    unsigned char *bytes;
    uint16_t reg;
    uint16_t end;
} Reader;

uint8_t readbytes_1(Reader *);
uint16_t readbytes_2(Reader *);
uint32_t readbytes_4(Reader *);
uint64_t readbytes_8(Reader *);

Reader *readClass(const char *);

#endif //MINIJVM_CLASSLOADER_H
