#include "mini_jvm/class_loader/class_obj.h"

#include <glib.h>
#include <stdlib.h>

#include "logging/log.h"
#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/constant_pool/link_resolver.h"

#include <stdlib.h>

err_vm co_load_cls_obj(co_cls_obj *cls, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    cls->access_flags = file->access_flags;
    E_HANDLE(cp_load_from_cls_file(&cls->cpool, file), ret_val, END);
    
    cp_utf8 cls_path;
    cp_utf8 sup_cls;
    err_vm cls_sig = cp_get_class_name(cls->cpool, file->this_class, &cls_path);
    E_HANDLE(cls_sig, ret_val, FREE_CPOOL);
    err_vm sup_sig = cp_get_class_name(cls->cpool, file->super_class, &sup_cls);
    E_HANDLE(sup_sig, ret_val, FREE_CPOOL);

    E_HANDLE(cf_copy_cp_utf8(&cls->cls_path, &cls_path), ret_val, FREE_CPOOL);
    E_HANDLE(cf_copy_cp_utf8(&cls->sup_cls, &sup_cls), ret_val, FREE_CLS_PATH);

    goto END;

FREE_SUP_PATH:
    free(cls->sup_cls.bytes);
FREE_CLS_PATH:
    free(cls->cls_path.bytes);
FREE_CPOOL:
    g_ptr_array_free(cls->cpool, true);
END:
    return ret_val;
}
