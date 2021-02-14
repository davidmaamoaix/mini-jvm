#ifndef MINIJVM_BYTESTREAM_H
#define MINIJVM_BYTESTREAM_H

#include <inttypes.h>

typedef struct ByteStream {
    uint8_t *bytes;
    uint32_t reg;
    uint32_t end;
} ByteStream;

uint8_t readbytes_1(ByteStream *);
uint16_t readbytes_2(ByteStream *);
uint32_t readbytes_4(ByteStream *);
uint64_t readbytes_8(ByteStream *);
uint8_t *readBytes(ByteStream *, uint32_t);

#endif //MINIJVM_BYTESTREAM_H
