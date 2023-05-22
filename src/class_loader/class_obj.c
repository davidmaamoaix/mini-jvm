#include "mini_jvm/class_loader/class_obj.h"

#include <glib.h>

#include "logging/log.h"
#include "mini_jvm/constant_pool/constant_pool.h"

#include <stdlib.h>

err_vm co_load_cls_obj(co_cls_obj *cls, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    cls->access_flags = file->access_flags;
    E_HANDLE(cp_load_from_cls_file(&cls->cpool, file), ret_val, END);
    
    log_debug("Size: %u", cls->cpool->len);

    goto END;

FREE_CPOOL:
    g_ptr_array_free(cls->cpool, true);
END:
    return ret_val;
}
