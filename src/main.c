#include <stdio.h>

#include "logging/log.h"
#include "mini_jvm/class_loader/class_format.h"
#include "mini_jvm/class_loader/class_reader.h"
#include "mini_jvm/utils/file_io.h"
#include "mini_jvm/utils/sreader.h"

int main() {
    // log_set_level(LOG_DEBUG);

    cf_cls_file file;
    err_vm read_sig = cr_read_cls_file("Cows.class", &file);

    printf("Signal: %d\n", read_sig);
}
