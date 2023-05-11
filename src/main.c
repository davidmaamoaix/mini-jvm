#include <stdio.h>

#include "mini_jvm/utils/file_io.h"
#include "mini_jvm/utils/sreader.h"

int main() {
    sreader reader;
    reader.curr = 0;

    err_vm sig = f_read_bytes("Cows.class", &reader.end, &reader.bytes);
    if (sig == -1) printf("Wat\n");

    printf("Hello World: %u\n", reader.end);
}
