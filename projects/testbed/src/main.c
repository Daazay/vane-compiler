#include "vanec/vanec.h"

#include <stdio.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("%d + %d = %d\n", 5, 10, sum(5, 10));

    return 0;
}