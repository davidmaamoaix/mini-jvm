#include "mini_jvm/constant_pool/constant_pool.h"

#include <stdlib.h>

#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/class_loader/class_format.h"

// TODO: fix memory freeing here
err_vm cp_load_from_cls_file(cp_pool *cp, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    cp->size = file->constant_pool_count;
    cp->pool = malloc(cp->size * sizeof(cp_info));
    E_MEM_HANDLE(cp->pool, ret_val, END);

    for (uint16_t i = 0; i < cp->size; i++) {
        if (file->constant_pool[i].tag == CONSTANT_Utf8) {
            cp_info *dest = cp->pool + i;
            cp_info *src = file->constant_pool + i;

            dest->tag = CONSTANT_Utf8;
            err_vm signal = cf_copy_cp_utf8(&dest->data.utf8, &src->data.utf8);
            E_HANDLE(signal, ret_val, FREE_CPOOL);
        } else {
            cp->pool[i] = file->constant_pool[i];
        }
    }

    goto END;

FREE_CPOOL:
    free(cp->pool);
END:
    return ret_val;
}