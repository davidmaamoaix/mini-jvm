#include "mini_jvm/class_loader/class.h"

#include <glib.h>
#include <stdlib.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/constant_pool/link_resolver.h"

#include <stdlib.h>

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

err_vm co_fprint_constant_pool(UFILE *out, co_cls_obj *obj) {
    err_vm ret_val = E_SUCC;

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

err_vm co_fprintf_fields(UFILE *out, co_cls_obj *obj) {
    err_vm ret_val = E_SUCC;

    

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

    E_HANDLE(co_fprint_constant_pool(out, obj), ret_val, END);

    u_fprintf(out, "{\n");
    co_fprintf_fields(out, obj);


    u_fprintf(out, "}\n");

END:
    return ret_val;
}
