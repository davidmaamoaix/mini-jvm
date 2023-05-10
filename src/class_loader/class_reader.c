#include "mini_jvm/class_loader/class_reader.h"

#include <stdlib.h>

err_vm sr_read_cls_file(sreader *reader, cls_file *file) {
    err_vm ret_val = E_SUC;

    E_PROP(sr_read_4(reader, &file->magic));
    E_PROP(sr_read_2(reader, &file->minor_version));
    E_PROP(sr_read_2(reader, &file->major_version));

    // Constant pool.
    // Constant pool starts at index 1.
    E_PROP(sr_read_2(reader, &file->constant_pool_count));
    file->constant_pool = malloc(file->constant_pool_count * sizeof(cp_info));
    E_MEM_PROP(file->constant_pool);
    for (uint16_t i = 1; i < file->constant_pool_count; i++) {
        err_vm sig = sr_read_cp_info(reader, file->constant_pool + i);
        E_HANDLE(sig, ret_val, FREE_CONST);
    }

    E_HANDLE(sr_read_2(reader, &file->access_flags), ret_val, FREE_CONST);
    E_HANDLE(sr_read_2(reader, &file->this_class), ret_val, FREE_CONST);
    E_HANDLE(sr_read_2(reader, &file->super_class), ret_val, FREE_CONST);

    // Implemented interfaces.
    E_HANDLE(sr_read_2(reader, &file->interfaces_count), ret_val, FREE_CONST);
    file->interfaces = malloc(file->interfaces_count * sizeof(uint16_t));
    E_MEM_HANDLE(file->interfaces, ret_val, FREE_CONST);
    for (uint16_t i = 0; i < file->interfaces_count; i++) {
        err_vm sig = sr_read_2(reader, file->interfaces + i);
        E_HANDLE(sig, ret_val, FREE_INTERFACES);
    }

    // Fields.
    E_HANDLE(sr_read_2(reader, &file->fields_count), ret_val, FREE_INTERFACES);
    file->fields = malloc(file->fields_count * sizeof(field_info));
    E_MEM_HANDLE(file->fields, ret_val, FREE_INTERFACES);
    for (uint16_t i = 0; i < file->fields_count; i++) {
        err_vm sig = sr_read_field_info(reader, file->fields + i);
        E_HANDLE(sig, ret_val, FREE_FIELDS);
    }

    // Methods.
    E_HANDLE(sr_read_2(reader, &file->methods_count), ret_val, FREE_FIELDS);
    file->methods = malloc(file->methods_count * sizeof(method_info));
    E_MEM_HANDLE(file->methods, ret_val, FREE_FIELDS);
    for (uint16_t i = 0; i < file->methods_count; i++) {
        err_vm sig = sr_read_method_info(reader, file->methods + i);
        E_HANDLE(sig, ret_val, FREE_METHODS);
    }

    // Attributes.
    E_HANDLE(sr_read_2(reader, &file->attributes_count), ret_val, FREE_METHODS);
    err_vm sig =
        sr_read_attribs(reader, file->attributes_count, &file->attributes);
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

err_vm sr_read_cp_info(sreader *reader, cp_info *info) {
    err_vm ret_val = E_SUC;
}

err_vm sr_read_attribute_info(sreader *reader, attribute_info *info) {
    err_vm ret_val = E_SUC;
}

err_vm sr_read_attribs(sreader *reader, uint16_t size, attribute_info **list) {
    err_vm ret_val = E_SUC;

    *list = malloc(size * sizeof(attribute_info));
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

err_vm sr_read_field_info(sreader *reader, field_info *info) {
    err_vm ret_val = E_SUC;
}

err_vm sr_read_method_info(sreader *reader, method_info *info) {
    err_vm ret_val = E_SUC;
}
