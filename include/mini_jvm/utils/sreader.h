#pragma once

#include <stdint.h>

#include "mini_jvm/error/err_vm.h"

typedef struct sreader {
    uint8_t *bytes;
    uint32_t curr;
    uint32_t end;
} sreader;

err_vm sr_read_1(sreader *reader, uint8_t *val);
err_vm sr_read_2(sreader *reader, uint16_t *val);
err_vm sr_read_4(sreader *reader, uint32_t *val);
err_vm sr_read_8(sreader *reader, uint64_t *val);
err_vm sr_read_bytes(sreader *reader, uint32_t size, uint8_t *val);
