#include <stdio.h>
#include <unicode/ustdio.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/class_loader/class_reader.h"
#include "mini_jvm/class_loader/class_obj.h"
#include "mini_jvm/utils/file_io.h"
#include "mini_jvm/utils/sreader.h"

int main() {
    // log_set_level(LOG_DEBUG);

    cf_cls_file file;
    err_vm read_sig = cr_read_cls_file("Cows.class", &file);
    printf("Read Signal: %d\n", read_sig);

    co_cls_obj obj;
    err_vm load_sig = co_load_cls_obj(&obj, &file);
    printf("Load Signal: %d\n", load_sig);

    err_vm print_sig = co_fprint_cls_obj(&obj, stdout);

    printf("Print Signal: %d\n", print_sig);
}
