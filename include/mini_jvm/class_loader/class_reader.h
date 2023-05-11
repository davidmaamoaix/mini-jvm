#pragma once

#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/error/err_vm.h"
#include "mini_jvm/utils/sreader.h"

err_vm sr_read_cls_file(sreader *reader, cf_cls_file *file);
err_vm sr_read_cp_info(sreader *reader, cf_cp_info *info);
err_vm sr_read_attribute_info(sreader *reader, cf_attribute_info *info);
err_vm sr_read_attrs(sreader *reader, uint16_t size, cf_attribute_info **list);
err_vm sr_read_field_info(sreader *reader, cf_field_info *info);
err_vm sr_read_method_info(sreader *reader, cf_method_info *info);
