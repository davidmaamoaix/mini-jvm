#include "mini_jvm/class_loader/class_obj.h"

#include <glib.h>
#include <stdlib.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/constant_pool/link_resolver.h"

#include <stdlib.h>

err_vm co_load_cls_obj(co_cls_obj *cls, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    cls->major_version = file->major_version;
    cls->minor_version = file->minor_version;
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

void cf_fprint_cp_utf8(cp_utf8 *str, UFILE *output) {
    u_fprintf(output, "%S", str->bytes);
}

err_vm cf_fprint_cls_obj(co_cls_obj *obj, FILE *output) {
    err_vm ret_val = E_SUCC;
    UFILE *out = u_finit(output, NULL, NULL);

    // Access level header.
    if (obj->access_flags & C_ACC_PUBLIC) {
        u_fprintf(out, "public ");
    }

    u_fprintf(out, "class %S\n", obj->cls_path.bytes);
    u_fprintf(out, "  minor version: %d\n", obj->minor_version);
    u_fprintf(out, "  major version: %d\n", obj->major_version);
    
    u_fprintf(out, "  flags: ");
    char flags[256];
    c_sprint_flags(obj->access_flags, flags);
    u_fprintf(out, "%s\n", flags);

    u_fprintf(out, "Constant pool:\n");
    for (uint16_t i = 1; i < obj->cpool->len; i++) {
        char tag[7] = "#";
        char num[6];
        sprintf(num, "%u", i);
        strcat(tag, num);

        cp_info *entry = g_ptr_array_index(obj->cpool, i);
        u_fprintf(out, "%6s = %-18s ", tag, cp_get_name(entry->tag));

        // Left align with 13 chars width.
        switch (entry->tag) {

        case CONSTANT_Utf8: {
            cf_fprint_cp_utf8(&entry->data.utf8, out);
            break;
        }

        case CONSTANT_Integer: {
            u_fprintf(out, "%-13d ", entry->data.integer.value);
            break;
        }

        case CONSTANT_Float: {
            char nums[64];
            sprintf(nums, "%.3ff", entry->data.float_.value);
            u_fprintf(out, "%-13s ", nums);
            break;
        }

        case CONSTANT_Long: {
            char nums[64];
            sprintf(nums, "%lldl", entry->data.long_.value);
            u_fprintf(out, "%-13s ", nums);
            break;
        }

        case CONSTANT_Double: {
            char nums[64];
            sprintf(nums, "%.3gd", entry->data.double_.value);
            u_fprintf(out, "%-13s ", nums);
            break;
        }

        case CONSTANT_Class: {
            uint16_t index = entry->data.class.name_index;
            u_fprintf(out, "#%-12u // ", index);

            cp_info path;
            err_vm sig = cp_expect(obj->cpool, index, CONSTANT_Utf8, &path);
            E_HANDLE(sig, ret_val, END);
            break;
        }

        default: {
            break;
        }
        }

        u_fprintf(out, "\n");
    }

END:
    return ret_val;
}
