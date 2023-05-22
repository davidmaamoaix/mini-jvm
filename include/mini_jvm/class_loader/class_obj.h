#pragma once

#include <stdint.h>
#include <unicode/ustring.h>
#include <glib.h>

#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/error/err_vm.h"

typedef struct {

} co_interface_obj;

typedef struct {

} co_method_obj;

typedef struct {

} co_field_obj;

typedef struct {
    uint16_t access_flags;
    GPtrArray *cpool; // Constant Pool.

    cp_utf8 *cls_path;
    cp_utf8 *parent_cls;

    GHashTable *interfaces;
    GHashTable *fields;
    GHashTable *methods;
} co_cls_obj;

err_vm co_load_cls_obj(co_cls_obj *cls, cf_cls_file *file);
