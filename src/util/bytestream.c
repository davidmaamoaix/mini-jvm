#include "bytestream.h"

#include <stdlib.h>
#include <stdio.h>

#include "util/debug.h"

uint8_t readbytes_1(ByteStream *stream) {
    if (stream->reg == stream->end) {
        ERROR("[Error] End of file reached\n");
        exit(5);
    }

    return stream->bytes[stream->reg++];
}

uint16_t readbytes_2(ByteStream *stream) {
    return (uint16_t) readbytes_1(stream) << 8 | readbytes_1(stream);
}

uint32_t readbytes_4(ByteStream *stream) {
    return (uint32_t) readbytes_2(stream) << 16 | readbytes_2(stream);
}

uint64_t readbytes_8(ByteStream *stream) {
    return (uint64_t) readbytes_4(stream) << 32 | readbytes_4(stream);
}

uint8_t *readBytes(ByteStream *reader, uint32_t size) {
    uint8_t *bytes = malloc(size * sizeof(unsigned char));

    if (bytes == NULL) {
        ERROR("[Error] Malloc failed when reading bytes\n");
        exit(12);
    }

    for (uint16_t i = 0; i < size; ++i) {
        bytes[i] = readbytes_1(reader);
    }

    return bytes;
}