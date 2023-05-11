#pragma once

#include <stdint.h>

#include "mini_jvm/error/err_vm.h"

err_vm f_read_bytes(const char *filename, uint32_t *size, uint8_t **buffer);
