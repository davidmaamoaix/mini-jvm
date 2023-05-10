#include <stdio.h>

#include "mini_jvm/utils/file_io.h"
#include "mini_jvm/utils/sreader.h"

int main() {
    sreader reader;
    reader.curr = 0;

    int64_t size = f_read_bytes("Cows.class", &reader.bytes);
    if (size == -1) printf("Wat\n");
    reader.end = (uint32_t) size;

    printf("Hello World: %u\n", reader.end);
}
