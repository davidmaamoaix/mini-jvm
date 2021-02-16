#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG

int main() {
    Class *class = loadClass("Foo.class");
    uint8_t good = verifyClass(class);

    printf("%d\n", good);

    freeClass(class);

    return 0;
}