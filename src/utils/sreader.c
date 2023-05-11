#include "mini_jvm/utils/sreader.h"

#include <string.h>

#include "mini_jvm/error/err_vm.h"

err_vm sr_read_uint(sreader *reader, uint32_t size, uint64_t *val) {
    if (reader->curr + size > reader->end)
        return E_EOFL;

    uint64_t acc = 0;
    for (int i = 0; i < size; i++) {
        acc <<= 8;
        acc |= reader->bytes[reader->curr++];
    }

    *val = acc;
    return E_SUCC;
}

err_vm sr_read_1(sreader *reader, uint8_t *val) {
    uint64_t acc;
    E_PROP(sr_read_uint(reader, 1, &acc));
    *val = (uint8_t)acc;

    return E_SUCC;
}

err_vm sr_read_2(sreader *reader, uint16_t *val) {
    uint64_t acc;
    E_PROP(sr_read_uint(reader, 2, &acc));
    *val = (uint16_t)acc;

    return E_SUCC;
}

err_vm sr_read_4(sreader *reader, uint32_t *val) {
    uint64_t acc;
    E_PROP(sr_read_uint(reader, 4, &acc));
    *val = (uint32_t)acc;

    return E_SUCC;
}

err_vm sr_read_8(sreader *reader, uint64_t *val) {
    uint64_t acc;
    E_PROP(sr_read_uint(reader, 8, &acc));
    *val = acc;

    return E_SUCC;
}

err_vm sr_read_bytes(sreader *reader, uint32_t size, uint8_t *val) {
    if (reader->curr + size > reader->end)
        return E_EOFL;

    memcpy(val, reader->bytes + reader->curr, size * sizeof(uint8_t));
    reader->curr += size;
    return E_SUCC;
}
