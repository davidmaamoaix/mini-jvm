#include "mini_jvm/class_loader/class_flags.h"

const char *cp_names[19] = {
    CP_INVALID,
    "Utf8",
    CP_INVALID,
    "Integer",
    "Float",
    "Long",
    "Double",
    "Class",
    "String",
    "Fieldref",
    "Methodref",
    "InterfaceMethodref",
    "NameAndType",
    CP_INVALID,
    CP_INVALID,
    "MethodHandle",
    "MethodType",
    CP_INVALID,
    "InvokeDynamic"
};

const char *cp_get_name(uint8_t tag) {
    if (tag > 18)
        return CP_INVALID;
    
    return cp_names[tag];
}
