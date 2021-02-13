#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG

int main() {
    Class *class = loadClass("Foo.class");


    freeClass(class);

    return 0;
}