#include "mini_jvm/constant_pool/constant_pool.h"

#include <stdlib.h>
#include <glib.h>

#include "mini_jvm/class_loader/class_flags.h"

/* void free_cp_entry(gpointer data) {
    cp_info *entry = (cp_info *)data;
    if (entry->tag == CONSTANT_Utf8) {
        free(entry->data.utf8.bytes);
    }

    free(entry);
}*/

/*err_vm cp_load_from_cls_file(GPtrArray **cp, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    *cp = g_ptr_array_new_with_free_func(free_cp_entry);
    for (uint16_t i = 0; i < file->constant_pool_count; i++) {
        cp_info *src = file->constant_pool + i;
        cp_info *dest = malloc(sizeof(cp_info));
        E_MEM_HANDLE(dest, ret_val, FREE_CPOOL);

        if (file->constant_pool[i].tag == CONSTANT_Utf8) {
            dest->tag = CONSTANT_Utf8;
            err_vm signal = cf_copy_cp_utf8(&dest->data.utf8, &src->data.utf8);
            E_HANDLE(signal, ret_val, FREE_CPOOL);
        } else {
            memcpy(dest, src, sizeof(cp_info));
        }

        g_ptr_array_add(*cp, dest);
    }

    goto END;

FREE_CPOOL:
    g_ptr_array_free(*cp, true);
END:
    return ret_val;
}*/