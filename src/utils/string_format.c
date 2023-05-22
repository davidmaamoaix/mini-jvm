#include "mini_jvm/utils/string_format.h"

#include <stdlib.h>
#include <unicode/ucnv.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/error/err_vm.h"

err_vm sf_decode_utf8(uint16_t size, uint8_t *bytes, cp_utf8 *str_info) {
    err_vm ret_val = E_SUCC;

    // Step 1: Convert `uint8_t *` to a `UChar32` string.

    // `size` is the upper-bound of the length of Unicode code points.
    uint16_t char_count = 0;
    UChar32 uni_chars[size];

    for (uint16_t i = 0; i < size;) {
        UChar32 curr;

        uint8_t a = bytes[i++];
        if (SF_IS_X_ENCODING_HEADER(a)) {
            curr = SF_DECODE_X(a);

        } else if (SF_IS_XY_ENCODING_HEADER(a)) {
            uint8_t b = bytes[i++];

            E_ASSERT_PROP(SF_IS_10_HEADER(b), E_UTFE);

            curr = SF_DECODE_XY(a, b);

        } else if (SF_IS_XYZ_ENCODING_HEADER(a)) {
            uint8_t b = bytes[i++];
            uint8_t c = bytes[i++];

            E_ASSERT_PROP(SF_IS_10_HEADER(b), E_UTFE);
            E_ASSERT_PROP(SF_IS_10_HEADER(c), E_UTFE);

            curr = SF_DECODE_XYZ(a, b, c);

        } else if (SF_IS_SUPP_HEADER(a)) {
            uint8_t b = bytes[i++];
            uint8_t c = bytes[i++];
            uint8_t d = bytes[i++];
            uint8_t e = bytes[i++];
            uint8_t f = bytes[i++];

            E_ASSERT_PROP(SF_IS_1010_HEADER(b), E_UTFE);
            E_ASSERT_PROP(SF_IS_10_HEADER(c), E_UTFE);
            E_ASSERT_PROP(SF_IS_SUPP_HEADER(d), E_UTFE);
            E_ASSERT_PROP(SF_IS_1011_HEADER(e), E_UTFE);
            E_ASSERT_PROP(SF_IS_10_HEADER(f), E_UTFE);

            curr = SF_DECODE_UVWXYZ(b, c, e, f);

        } else {
            log_error("Unexpected character in unicode: %#X", a);
            ret_val = E_UTFE;
            goto END;
        }

        uni_chars[char_count++] = curr;
    }

    // Step 2: Convert a `UChar32` string to a `UChar` string.
    int32_t out_size = 0;
    UChar *out_str = malloc(size * sizeof(UChar));
    E_MEM_HANDLE(out_str, ret_val, END);
    UErrorCode status = U_ZERO_ERROR;

    u_strFromUTF32(out_str, size, &out_size, uni_chars, char_count, &status);
    if (U_FAILURE(status)) {
        ret_val = E_UTFE;
        goto FREE_STR;
    }

    str_info->length = out_size;
    str_info->bytes = out_str;

    goto END;

FREE_STR:
    free(out_str);
END:
    return ret_val;
}
