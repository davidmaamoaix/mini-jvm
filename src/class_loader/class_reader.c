#include "mini_jvm/class_loader/class_reader.h"

#include <stdlib.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_flags.h"
#include "mini_jvm/utils/file_io.h"

err_vm cr_read_cls_file(const char *path, cf_cls_file *file) {
    log_debug("Reading class file %s", path);
    err_vm ret_val = E_SUCC;

    sreader reader;
    reader.curr = 0;

    E_PROP(f_read_bytes(path, &reader.end, &reader.bytes));
    log_debug("Size of %s: %u bytes", path, reader.end);
    log_debug("Parsing class file %s", path);
    E_HANDLE(sr_read_cls_file(&reader, file), ret_val, FREE_READER);

    if (reader.curr != reader.end) {
        log_error("File not fully consumed: %u/%u", reader.curr, reader.end);
        ret_val = E_EOFL;
        goto FREE_READER;
    }

    log_debug("%s successfully parsed", path);

    goto END;

FREE_READER:
    free(reader.bytes);
END:
    return ret_val;
}

err_vm sr_read_cls_file(sreader *reader, cf_cls_file *file) {
    err_vm ret_val = E_SUCC;

    E_PROP(sr_read_4(reader, &file->magic));
    log_trace("Magic number 0x%X", file->magic);

    E_PROP(sr_read_2(reader, &file->minor_version));
    E_PROP(sr_read_2(reader, &file->major_version));
    log_trace("Version %d.%d", file->major_version, file->minor_version);

    // Constant pool.
    // Constant pool starts at index 1.
    uint16_t cp_size;
    E_PROP(sr_read_2(reader, &cp_size));
    log_debug("Constant pool size: %u", cp_size);

    file->constant_pool_count = cp_size;
    file->constant_pool = malloc(cp_size * sizeof(cp_info));
    E_MEM_PROP(file->constant_pool);
    for (uint16_t i = 1; i < file->constant_pool_count; i++) {
        err_vm sig = sr_read_cp_info(reader, file->constant_pool + i, &i);
        E_HANDLE(sig, ret_val, FREE_CONST);
    }

    E_HANDLE(sr_read_2(reader, &file->access_flags), ret_val, FREE_CONST);
    log_trace("Access flags: %u", file->access_flags);
    E_HANDLE(sr_read_2(reader, &file->this_class), ret_val, FREE_CONST);
    log_trace("Class name index #%u", file->this_class);
    E_HANDLE(sr_read_2(reader, &file->super_class), ret_val, FREE_CONST);
    log_trace("Super class index #%u", file->super_class);

    // Implemented interfaces.
    E_HANDLE(sr_read_2(reader, &file->interfaces_count), ret_val, FREE_CONST);
    log_debug("Interfaces count: %u", file->interfaces_count);

    file->interfaces = malloc(file->interfaces_count * sizeof(uint16_t));
    E_MEM_HANDLE(file->interfaces, ret_val, FREE_CONST);
    for (uint16_t i = 0; i < file->interfaces_count; i++) {
        err_vm sig = sr_read_2(reader, file->interfaces + i);
        E_HANDLE(sig, ret_val, FREE_INTERFACES);
    }

    // Fields.
    E_HANDLE(sr_read_2(reader, &file->fields_count), ret_val, FREE_INTERFACES);
    log_debug("Fields count: %u", file->fields_count);

    file->fields = malloc(file->fields_count * sizeof(cf_field_info));
    E_MEM_HANDLE(file->fields, ret_val, FREE_INTERFACES);
    for (uint16_t i = 0; i < file->fields_count; i++) {
        err_vm sig = sr_read_field_info(reader, file->fields + i);
        E_HANDLE(sig, ret_val, FREE_FIELDS);
    }

    // Methods.
    E_HANDLE(sr_read_2(reader, &file->methods_count), ret_val, FREE_FIELDS);
    log_debug("Methods count: %u", file->methods_count);

    file->methods = malloc(file->methods_count * sizeof(cf_method_info));
    E_MEM_HANDLE(file->methods, ret_val, FREE_FIELDS);
    for (uint16_t i = 0; i < file->methods_count; i++) {
        err_vm sig = sr_read_method_info(reader, file->methods + i);
        E_HANDLE(sig, ret_val, FREE_METHODS);
    }

    // Attributes.
    uint16_t attrs_count;
    E_HANDLE(sr_read_2(reader, &attrs_count), ret_val, FREE_METHODS);
    log_debug("Attributes count: %u", attrs_count);

    file->attributes_count = attrs_count;
    err_vm sig = sr_read_attrs(reader, attrs_count, &file->attributes);
    E_HANDLE(sig, ret_val, FREE_METHODS);

    goto END;

FREE_METHODS:
    free(file->methods);
FREE_FIELDS:
    free(file->fields);
FREE_INTERFACES:
    free(file->interfaces);
FREE_CONST:
    free(file->constant_pool);
END:
    return ret_val;
}

/* `uint16_t *iter` is a pointer to the accumulating index when looping through
 * the constant pool. This is needed because some entry would change the
 * accumulation of the index (e.g., `CONSTANT_Long_info` and
 * `CONSTANT_Double_info` takes up 2 entries).*/
err_vm sr_read_cp_info(sreader *reader, cp_info *info, uint16_t *iter) {
    err_vm ret_val = E_SUCC;
    E_PROP(sr_read_1(reader, &info->tag));
    log_trace("Index %u: %s", *iter, cp_get_name(info->tag));

    switch (info->tag) {
    case CONSTANT_Utf8: {
        uint16_t length;
        E_PROP(sr_read_2(reader, &length));

        info->data.utf8.length = length + 1;
        info->data.utf8.bytes = malloc(length * sizeof(uint8_t));
        E_MEM_PROP(info->data.utf8.bytes);
        info->data.utf8.bytes[length] = '\0';
        printf("%s\n", info->data.utf8.bytes);

        err_vm sig = sr_read_bytes(reader, length, info->data.utf8.bytes);
        E_HANDLE(sig, ret_val, UTF8_FREE_BYTES);
        goto UTF8_END;

    UTF8_FREE_BYTES:
        free(info->data.utf8.bytes);
    UTF8_END:
        break;
    }

    // TODO: simplify this part by passing in `&blah.value` directly.
    case CONSTANT_Integer: {
        uint32_t value = 0;
        E_PROP(sr_read_4(reader, &value));
        info->data.integer.value = (int32_t)value;
        break;
    }

    case CONSTANT_Float: {
        uint32_t value = 0;
        E_PROP(sr_read_4(reader, &value));
        info->data.float_.value = *((float *)&value);
        break;
    }

    case CONSTANT_Long: {
        uint64_t value = 0;
        E_PROP(sr_read_8(reader, &value));
        info->data.long_.value = (int64_t)value;
        ++*iter;
        break;
    }

    case CONSTANT_Double: {
        uint64_t value = 0;
        E_PROP(sr_read_8(reader, &value));
        info->data.double_.value = *((double *)&value);
        ++*iter;
        break;
    }

    case CONSTANT_Class: {
        E_PROP(sr_read_2(reader, &info->data.class.name_index));
        break;
    }

    case CONSTANT_String: {
        E_PROP(sr_read_2(reader, &info->data.string.string_index));
        break;
    }

    case CONSTANT_Fieldref:
    case CONSTANT_Methodref:
    case CONSTANT_InterfaceMethodref: {
        E_PROP(sr_read_2(reader, &info->data.ref.class_index));
        E_PROP(sr_read_2(reader, &info->data.ref.name_and_type_index));
        break;
    }

    case CONSTANT_NameAndType: {
        E_PROP(sr_read_2(reader, &info->data.name_and_type.name_index));
        uint16_t *desc_ptr = &info->data.name_and_type.descriptor_index;
        E_PROP(sr_read_2(reader, desc_ptr));
        break;
    }

    case CONSTANT_MethodHandle: {
        uint8_t *ref_kind_ptr = &info->data.method_handle.reference_kind;
        uint16_t *ref_idx_ptr = &info->data.method_handle.reference_index;
        E_PROP(sr_read_1(reader, ref_kind_ptr));
        E_PROP(sr_read_2(reader, ref_idx_ptr));
        break;
    }

    case CONSTANT_MethodType: {
        uint16_t *desc_ptr = &info->data.method_type.descriptor_index;
        E_PROP(sr_read_2(reader, desc_ptr));
        break;
    }

    case CONSTANT_InvokeDynamic: {
        uint16_t *attr_ptr =
            &info->data.invoke_dynamic.bootstrap_method_attr_index;
        uint16_t *tp_ptr = &info->data.invoke_dynamic.name_and_type_index;
        E_PROP(sr_read_2(reader, attr_ptr));
        E_PROP(sr_read_2(reader, tp_ptr));
        break;
    }

    default: {
        ret_val = E_IBTC;
        break;
    }
    }

    return ret_val;
}

err_vm sr_read_attribute_info(sreader *reader, cf_attribute_info *info) {
    err_vm ret_val = E_SUCC;

    E_PROP(sr_read_2(reader, &info->attribute_name_index));
    E_PROP(sr_read_4(reader, &info->attribute_length));
    info->info = malloc(info->attribute_length * sizeof(uint8_t));
    E_MEM_PROP(info->info);
    err_vm sig = sr_read_bytes(reader, info->attribute_length, info->info);
    E_HANDLE(sig, ret_val, FREE_INFO);

    goto END;

FREE_INFO:
    free(info->info);
END:
    return ret_val;
}

err_vm sr_read_attrs(sreader *reader, uint16_t size, cf_attribute_info **list) {
    err_vm ret_val = E_SUCC;

    *list = malloc(size * sizeof(cf_attribute_info));
    E_MEM_PROP(*list);
    for (uint16_t i = 0; i < size; i++) {
        err_vm sig = sr_read_attribute_info(reader, *list + i);
        E_HANDLE(sig, ret_val, FREE_ATTRIBUTES);
    }

    goto END;

FREE_ATTRIBUTES:
    free(*list);
END:
    return ret_val;
}

err_vm sr_read_field_info(sreader *reader, cf_field_info *info) {
    E_PROP(sr_read_2(reader, &info->access_flags));
    E_PROP(sr_read_2(reader, &info->name_index));
    E_PROP(sr_read_2(reader, &info->descriptor_index));
    E_PROP(sr_read_2(reader, &info->attributes_count));
    err_vm sig =
        sr_read_attrs(reader, info->attributes_count, &info->attributes);
    E_PROP(sig);

    return E_SUCC;
}

err_vm sr_read_method_info(sreader *reader, cf_method_info *info) {
    E_PROP(sr_read_2(reader, &info->access_flags));
    E_PROP(sr_read_2(reader, &info->name_index));
    E_PROP(sr_read_2(reader, &info->descriptor_index));
    E_PROP(sr_read_2(reader, &info->attributes_count));
    err_vm sig =
        sr_read_attrs(reader, info->attributes_count, &info->attributes);
    E_PROP(sig);

    return E_SUCC;
}
