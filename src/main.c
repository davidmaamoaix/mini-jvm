#include <stdio.h>

#include "loader/classloader.h"

#define DEBUG 1

int main() {
    Class *class = loadClass("Foo.class");



    return 0;
}