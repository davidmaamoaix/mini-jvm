#include "mini_jvm/class_loader/class_format.h"

#include <stdlib.h>
#include <unicode/ustring.h>
#include <unicode/ustdio.h>

#include "mini_jvm/error/err_vm.h"
#include "mini_jvm/class_loader/class_obj.h"

err_vm cf_copy_cp_utf8(cp_utf8 *dest, cp_utf8 *src) {
    err_vm ret_val = E_SUCC;

    dest->length = src->length;
    dest->bytes = malloc(dest->length * sizeof(UChar));
    E_MEM_HANDLE(dest->bytes, ret_val, END);
    u_strncpy(dest->bytes, src->bytes, dest->length);

END:
    return ret_val;
}
