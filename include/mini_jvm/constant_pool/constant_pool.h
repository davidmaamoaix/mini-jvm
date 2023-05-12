#pragma once

#include "mini_jvm/class_loader/class_format.h"

typedef struct {
    uint16_t size;
    cp_info *pool;
} c_pool;
