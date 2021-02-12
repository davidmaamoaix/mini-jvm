#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG 1

int main() {
    loadClass("Foo.class");

    printf("\n%u", (unsigned int) (0 - 7));

    return 0;
}