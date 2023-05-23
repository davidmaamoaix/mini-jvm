#include "mini_jvm/class_loader/class_flags.h"

#include <unicode/ustdio.h>

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

void c_sprint_flags(uint16_t value, char *out) {
    char *begin = out;

    if (value & C_ACC_PUBLIC)
        out += sprintf(out, "ACC_PUBLIC, ");
    if (value & C_ACC_FINAL)
        out += sprintf(out, "ACC_FINAL, ");
    if (value & C_ACC_SUPER)
        out += sprintf(out, "ACC_SUPER, ");
    if (value & C_ACC_INTERFACE)
        out += sprintf(out, "ACC_INTERFACE, ");
    if (value & C_ACC_ABSTRACT)
        out += sprintf(out, "ACC_ABSTRACT, ");
    if (value & C_ACC_SYNTHETIC)
        out += sprintf(out, "ACC_SYNTHETIC, ");
    if (value & C_ACC_ANNOTATION)
        out += sprintf(out, "ACC_ANNOTATION, ");
    if (value & C_ACC_ENUM)
        out += sprintf(out, "ACC_SYNTHETIC, ");

    if (begin != out) *(out - 2) = '\0';
}
