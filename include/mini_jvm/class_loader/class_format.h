#pragma once

#include <stdint.h>
#include <stdio.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>

#include "mini_jvm/error/err_vm.h"

/* While the `cp_info` contents in the constant pool all share similar byte
 * format and therefore can be easily merged into a single union with two
 * branches, I feel like writing them out explicitly grants more readability
 * (all following structs with the prefix `cf_cp_`). After all this is just a
 * JVM demonstration ;) */

// CONSTANT_Class
typedef struct {
    uint16_t name_index;
} cp_class;

// CONSTANT_Fieldref, CONSTANT_Methodref, CONSTANT_InterfaceMethodref
typedef struct {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cp_ref;

// CONSTANT_String_info
typedef struct {
    uint16_t string_index;
} cp_string;

// CONSTANT_Integer_info
typedef struct {
    int32_t value;
} cp_integer;

// CONSTANT_Float_info
typedef struct {
    float value;
} cp_float;

// CONSTANT_Long_info
typedef struct {
    int64_t value;
} cp_long;

// CONSTANT_Double_info
typedef struct {
    double value;
} cp_double;

// CONSTANT_NameAndType_info
typedef struct {
    uint16_t name_index;
    uint16_t descriptor_index;
} cp_name_and_type;

// CONSTANT_Utf8_info
typedef struct {
    /* 32-bit integer is needed as the max size of a constant string is a 15-bit
     * integer, but due to `UChar`'s encoding the actual size may exceed that.
     */
    uint32_t length;
    UChar *bytes;
} cp_utf8;

// CONSTANT_MethodHandle_info
typedef struct {
    uint8_t reference_kind;
    uint16_t reference_index;
} cp_method_handle;

// CONSTANT_MethodType_info
typedef struct {
    uint16_t descriptor_index;
} cp_method_type;

// CONSTANT_InvokeDynamic_info
typedef struct {
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} cp_invoke_dynamic;

/* `cp_info` is used directly in the constant pool, thus the lack of `cf_`
 * prefix in its name.*/
typedef struct {
    uint8_t tag;
    union {
        cp_class class;
        cp_ref ref;
        cp_string string;
        cp_integer integer;
        cp_float float_;
        cp_long long_;
        cp_double double_;
        cp_name_and_type name_and_type;
        cp_utf8 utf8;
        cp_method_handle method_handle;
        cp_method_type method_type;
        cp_invoke_dynamic invoke_dynamic;
    } data;
} cp_info;

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t *info;
} cf_attribute_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;

    uint16_t attributes_count;
    cf_attribute_info *attributes;
} cf_field_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;

    uint16_t attributes_count;
    cf_attribute_info *attributes;
} cf_method_info;

typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;

    uint16_t constant_pool_count;
    cp_info *constant_pool;

    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    uint16_t interfaces_count;
    uint16_t *interfaces;

    uint16_t fields_count;
    cf_field_info *fields;

    uint16_t methods_count;
    cf_method_info *methods;

    uint16_t attributes_count;
    cf_attribute_info *attributes;
} cf_cls_file;

err_vm cf_copy_cp_utf8(cp_utf8 *dest, cp_utf8 *src);
void cf_fprint_cp_utf8(cp_utf8 *str, UFILE *output);
void cf_fprint_cls_obj(co_cls_obj *obj, UFILE *output);
