#include "vanec/utils/defines.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error_exit(i32 code) {
    exit(code);
}

void error_exit_with_msg(i32 code, const char* msg, ...) {
    assert(msg != NULL);
    va_list args;
    va_start(args, msg);
    printf("internal error: ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
    exit(code);
}