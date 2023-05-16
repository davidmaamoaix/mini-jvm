#include "mini_jvm/utils/string_format.h"

#include <stdlib.h>
#include <unicode/ucnv.h>
#include <unicode/ustring.h>

#include "logging/log.h"
#include "mini_jvm/error/err_vm.h"

err_vm sf_decode_utf8(uint16_t size, uint8_t *bytes, uint8_t **out_str) {
    err_vm ret_val = E_SUCC;
    uint16_t out_size = 0;
    *out_str = malloc(size * sizeof(uint8_t));
    E_MEM_PROP(out_str);
    
    for (uint16_t i = 0; i < size; ) {
        UChar32 curr;

        uint8_t a = bytes[i++];
        if (SF_IS_X_ENCODING_HEADER(a)) {
            curr = SF_DECODE_X(a);

        } else if (SF_IS_XY_ENCODING_HEADER(a)) {
            uint8_t b = bytes[i++];

            E_ASSERT_HANDLE(SF_IS_10_HEADER(b), E_UTFE, ret_val, FREE_STR);

            curr = SF_DECODE_XY(a, b);

        } else if (SF_IS_XYZ_ENCODING_HEADER(a)) {
            uint8_t b = bytes[i++];
            uint8_t c = bytes[i++];

            E_ASSERT_HANDLE(SF_IS_10_HEADER(b), E_UTFE, ret_val, FREE_STR);
            E_ASSERT_HANDLE(SF_IS_10_HEADER(c), E_UTFE, ret_val, FREE_STR);

            curr = SF_DECODE_XYZ(a, b, c);

        } else if (SF_IS_SUPP_HEADER(a)) {
            uint8_t b = bytes[i++];
            uint8_t c = bytes[i++];
            uint8_t d = bytes[i++];
            uint8_t e = bytes[i++];
            uint8_t f = bytes[i++];

            E_ASSERT_HANDLE(SF_IS_1010_HEADER(b), E_UTFE, ret_val, FREE_STR);
            E_ASSERT_HANDLE(SF_IS_10_HEADER(c), E_UTFE, ret_val, FREE_STR);
            E_ASSERT_HANDLE(SF_IS_SUPP_HEADER(d), E_UTFE, ret_val, FREE_STR);
            E_ASSERT_HANDLE(SF_IS_1011_HEADER(e), E_UTFE, ret_val, FREE_STR);
            E_ASSERT_HANDLE(SF_IS_10_HEADER(f), E_UTFE, ret_val, FREE_STR);

            curr = SF_DECODE_UVWXYZ(b, c, e, f);

        } else {
            log_error("Unexpected character in unicode: %#X", a);
            ret_val = E_UTFE;
            goto FREE_STR;
        }

        printf("%d ", curr);
    }

    printf("\n");

FREE_STR:
    free(*out_str);
END:
    return ret_val;
}
