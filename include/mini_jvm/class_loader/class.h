#pragma once

#include <glib.h>
#include <stdint.h>
#include <unicode/ustring.h>

#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/error/err_vm.h"

typedef struct {

} co_interface_obj;

typedef struct {

} co_method_obj;

typedef struct {

} co_field_obj;

typedef struct {
    uint16_t minor_version;
    uint16_t major_version;

    uint16_t access_flags;
    GPtrArray *cpool; // Constant Pool.

    cp_utf8 cls_path;
    cp_utf8 sup_cls;

    GHashTable *interfaces;
    GHashTable *fields;
    GHashTable *methods;
} co_cls_obj;
