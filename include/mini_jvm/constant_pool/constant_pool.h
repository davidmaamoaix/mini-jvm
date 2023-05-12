#pragma once

#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/error/err_vm.h"

typedef struct {
    uint16_t size;
    cp_info *pool;
} c_pool;

err_vm cp_load_from_cls_file(cf_cls_file *file, c_pool *cp);
