#pragma once

#include <stdint.h>
#include <unicode/ustring.h>

#include "mini_jvm/error/err_vm.h"
#include "mini_jvm/class_loader/class_format.h"

// String format byte masks.
#define SF_HIGH_1 0b10000000
#define SF_HIGH_2 0b11000000
#define SF_HIGH_3 0x11100000
#define SF_HIGH_4 0x11110000

// Byte encoding header check.
#define SF_IS_SUPP_HEADER(x)         ((x) == 0b11101101)
#define SF_IS_X_ENCODING_HEADER(x)   (((x)&SF_HIGH_1) == 0)
#define SF_IS_XY_ENCODING_HEADER(x)  (((x)&SF_HIGH_3) == 0b11000000)
#define SF_IS_XYZ_ENCODING_HEADER(x) (((x)&SF_HIGH_4) == 0b11100000)

// Encoding assertions.
#define SF_IS_10_HEADER(x)   (((x)&SF_HIGH_2) == 0b10000000)
#define SF_IS_1010_HEADER(x) (((x)&SF_HIGH_2) == 0b10100000)
#define SF_IS_1011_HEADER(x) (((x)&SF_HIGH_2) == 0b10110000)

// Encoding conversion.
#define SF_DECODE_X(x)     ((x)&0b01111111)
#define SF_DECODE_XY(x, y) ((((x)&0b00011111) << 6) + ((y)&0b00111111))
#define SF_DECODE_XYZ(x, y, z)                                                 \
    ((((x)&0b00001111) << 12) + (((y)&0b00111111) << 6) + ((z)&0b00111111))
#define SF_DECODE_UVWXYZ(v, w, y, z)                                           \
    (0x10000 + (((v)&0b00001111) << 16) + (((w)&0b00111111) << 10) +           \
     (((y)&0b00001111) << 6) + ((z)&0b00111111))

err_vm sf_decode_utf8(uint16_t size, uint8_t *bytes, cp_utf8 *str_info);
