#pragma once

typedef enum err_vm {
    E_SUC = 0,  // No error.
    E_EOF = 11, // End of file.
    E_MEM = 15  // Out of memory.
} err_vm;

// Propagates error from a function call.
#define E_PROP(call)                                                           \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUC)                                                   \
            return _errno;                                                     \
    } while (0)

// Handles an error with `goto` and sets the `ret_var` to the error code.
#define E_HANDLE(call, ret_val, label)                                         \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUC) {                                                 \
            ret_val = _errno;                                                  \
            goto label;                                                        \
        }                                                                      \
    }
