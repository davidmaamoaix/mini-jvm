#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG

int main() {
    Class *class = loadClass("Foo.class");

    printf("%d\n", class->methods[0].attrs[0].attrLen);

    freeClass(class);

    return 0;
}