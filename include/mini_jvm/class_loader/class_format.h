#pragma once

#include <stdint.h>
#include <stdio.h>

/* While the `cf_cp_info` contents in the constant pool all share similar byte
 * format and therefore can be easily merged into a single union with two
 * branches, I feel like writing them out explicitly grants more readability
 * (all following structs with the prefix `cf_cp_`). After all this is just a
 * JVM demonstration ;) */

// CONSTANT_Class
typedef struct {
    uint16_t name_index;
} cf_cp_class;

// CONSTANT_Fieldref, CONSTANT_Methodref, CONSTANT_InterfaceMethodref
typedef struct {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cf_cp_ref;

// CONSTANT_String_info
typedef struct {
    uint16_t string_index;
} cf_cp_string;

// CONSTANT_Integer_info, CONSTANT_Float_info
typedef struct {
    uint32_t bytes;
} cf_cp_number;

// CONSTANT_Long_info, CONSTANT_Double_info
typedef struct {
    uint32_t high_bytes;
    uint32_t low_bytes;
} cf_cp_large_number;

// CONSTANT_NameAndType_info
typedef struct {
    uint16_t name_index;
    uint16_t descriptor_index;
} cf_cp_name_and_type;

// CONSTANT_Utf8_info
typedef struct {
    uint32_t length;
    uint8_t *bytes;
} cf_cp_utf8;

// CONSTANT_MethodHandle_info
typedef struct {
    uint8_t reference_kind;
    uint16_t reference_index;
} cf_cp_method_handle;

// CONSTANT_MethodType_info
typedef struct {
    uint16_t descriptor_index;
} cf_cp_method_type;

// CONSTANT_InvokeDynamic_info
typedef struct {
    uint16_t bootstrap_method_attr_index;
    uint16_t name_and_type_index;
} cf_cp_invoke_dynamic;

typedef struct {
    uint8_t tag;
    union {

    } data;
} cf_cp_info;

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
    cf_cp_info *constant_pool;

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

void print_class_file(cf_cls_file *file, FILE *output);
