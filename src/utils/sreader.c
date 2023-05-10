#include "mini_jvm/utils/sreader.h"

// TODO: add proper error propagation and just memcpy the reading cuz this is
// rly bad currently.

uint8_t sr_read_1(sreader *reader) {
    if (reader->curr >= reader->end) {
        // TODO: error.
    }

    return reader->bytes[reader->curr++];
}

uint16_t sr_read_2(sreader *reader) {
    return (uint16_t) readbytes_1(reader) << 8 | readbytes_1(reader);
}

uint32_t sr_read_4(sreader *reader) {
    return (uint32_t) readbytes_2(reader) << 16 | readbytes_2(reader);
}

uint64_t sr_read_8(sreader *reader) {
    return (uint64_t) readbytes_4(reader) << 32 | readbytes_4(reader);
}

uint8_t *sr_read_bytes(sreader *reader, uint32_t size) {
    uint8_t *bytes = malloc(size * sizeof(uint8_t));

    for (uint16_t i = 0; i < size; ++i) {
        bytes[i] = sr_read_1(reader);
    }

    return bytes;
}
