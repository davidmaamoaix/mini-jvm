#pragma once

#include <unicode/ustring.h>

#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/error/err_vm.h"

err_vm cp_expect(c_pool *cp, uint16_t index, uint8_t type, cp_info *ret_val);
err_vm cp_get_class_name(c_pool *cp, uint16_t index, UChar **ret_val);
