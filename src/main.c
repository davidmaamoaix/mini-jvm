#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG

int main() {
    Class *class = loadClass("Foo.class");

    printf("%d\n", class->fields[0].descriptorIndex);

    return 0;
}