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

err_vm co_fprint_name_type(UFILE *out, GPtrArray *cp, cp_name_and_type *entry) {
    err_vm ret_val = E_SUCC;

    uint16_t name_idx = entry->name_index;
    uint16_t desc_idx = entry->descriptor_index;
    err_vm sig;
    cp_info name_info;
    cp_info desc_info;
    sig = cp_expect(cp, name_idx, CONSTANT_Utf8, &name_info);
    E_HANDLE(sig, ret_val, END);
    sig = cp_expect(cp, desc_idx, CONSTANT_Utf8, &desc_info);
    E_HANDLE(sig, ret_val, END);

    u_fprintf(out, "%S:", name_info.data.utf8.bytes);
    u_fprintf(out, "%S", desc_info.data.utf8.bytes);

END:
    return ret_val;
}

err_vm co_fprint_cls_obj(co_cls_obj *obj, FILE *output) {
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
    bool skip = false;
    for (uint16_t i = 1; i < obj->cpool->len; i++) {
        if (skip) {
            skip = false;
            continue;
        }

        char tag[7] = "#";
        char num[6];
        sprintf(num, "%u", i);
        strcat(tag, num);

        cp_info *entry = g_ptr_array_index(obj->cpool, i);
        u_fprintf(out, "%6s = %-18s ", tag, cp_get_name(entry->tag));

        // Left align with 13 chars width.
        switch (entry->tag) {

        case CONSTANT_Utf8: {
            u_fprintf(out, "%S", entry->data.utf8.bytes);
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
            skip = true;

            char nums[64];
            sprintf(nums, "%lldl", entry->data.long_.value);
            u_fprintf(out, "%-13s ", nums);
            break;
        }

        case CONSTANT_Double: {
            skip = true;

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
            u_fprintf(out, "%S", path.data.utf8.bytes);

            break;
        }

        case CONSTANT_String: {
            uint16_t index = entry->data.string.string_index;
            u_fprintf(out, "#%-12u // ", index);

            cp_info path;
            err_vm sig = cp_expect(obj->cpool, index, CONSTANT_Utf8, &path);
            E_HANDLE(sig, ret_val, END);
            u_fprintf(out, "%S", path.data.utf8.bytes);

            break;
        }

        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref: {
            char ref[64];
            uint16_t cls_idx = entry->data.ref.class_index;
            uint16_t name_idx = entry->data.ref.name_and_type_index;
            sprintf(ref, "#%u.#%u", cls_idx, name_idx);
            u_fprintf(out, "%-13s // ", ref);

            cp_utf8 cls;
            cp_info name;
            err_vm sig;
            sig = cp_get_class_name(obj->cpool, cls_idx, &cls);
            E_HANDLE(sig, ret_val, END);
            u_fprintf(out, "%S.", cls.bytes);

            sig = cp_expect(obj->cpool, name_idx, CONSTANT_NameAndType, &name);
            E_HANDLE(sig, ret_val, END);
            co_fprint_name_type(out, obj->cpool, &name.data.name_and_type);

            break;
        }

        case CONSTANT_NameAndType: {
            char ref[64];
            uint16_t name_idx = entry->data.name_and_type.name_index;
            uint16_t desc_idx = entry->data.name_and_type.descriptor_index;
            sprintf(ref, "#%u.#%u", name_idx, desc_idx);
            u_fprintf(out, "%-13s // ", ref);

            co_fprint_name_type(out, obj->cpool, &entry->data.name_and_type);
            
            break;
        }

        case CONSTANT_MethodHandle: {
            // TODO: do this.
            break;
        }

        case CONSTANT_MethodType: {
            uint16_t idx = entry->data.method_type.descriptor_index;
            u_fprintf(out, "#%-12d // ", idx);
            
            cp_info desc_info;
            err_vm sig = cp_expect(obj->cpool, idx, CONSTANT_Utf8, &desc_info);
            E_HANDLE(sig, ret_val, END);

            u_fprintf(out, "%S", desc_info.data.utf8.bytes);

            break;
        }

        case CONSTANT_InvokeDynamic: {
            uint16_t b = entry->data.invoke_dynamic.bootstrap_method_attr_index;
            uint16_t name_idx = entry->data.invoke_dynamic.name_and_type_index;

            char ref[64];
            sprintf(ref, "#%u.#%u", b, name_idx);
            u_fprintf(out, "%-13s // #%u:", ref, b);

            cp_info name;
            err_vm sig;
            sig = cp_expect(obj->cpool, name_idx, CONSTANT_NameAndType, &name);
            E_HANDLE(sig, ret_val, END);

            co_fprint_name_type(out, obj->cpool, &name.data.name_and_type);

            break;
        }

        default: {
            log_error("Invalid constant pool tag");
            ret_val = E_IBTC;
            goto END;
        }
        }

        u_fprintf(out, "\n");
    }

END:
    return ret_val;
}
