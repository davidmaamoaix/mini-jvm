#pragma once

#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/error/err_vm.h"
#include "mini_jvm/utils/sreader.h"

err_vm sr_read_cls_file(sreader *reader, cls_file *file);
err_vm sr_read_cp_info(sreader *reader, cp_info *info);
err_vm sr_read_attribute_info(sreader *reader, attribute_info *info);
err_vm sr_read_attribs(sreader *reader, uint16_t size, attribute_info **list);
err_vm sr_read_field_info(sreader *reader, field_info *info);
err_vm sr_read_method_info(sreader *reader, method_info *info);
