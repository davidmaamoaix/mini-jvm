#pragma once

#include <glib.h>
#include <unicode/ustring.h>

#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/constant_pool/constant_pool.h"
#include "mini_jvm/error/err_vm.h"

err_vm cp_expect(GPtrArray *cp, uint16_t id, uint8_t type, cp_info *out);
err_vm cp_get_class_name(GPtrArray *cp, uint16_t id, UChar **out);
