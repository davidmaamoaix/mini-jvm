#pragma once

typedef enum err_vm {
    E_SUC = 0,  // No error.
    E_EOF = 11, // End of file.
    E_MEM = 15  // Out of memory.
} err_vm;

// Checks memory allocation with error propagation.
#define E_MEM_PROP(val)                                                        \
    if (val == NULL)                                                           \
        return E_MEM;

// Checls memory allocation and handles it with `goto` and sets `ret_val`.
#define E_MEM_HANDLE(val, ret_val, label)                                      \
    if (val == NULL) {                                                         \
        ret_val = E_MEM;                                                       \
        goto label;                                                            \
    }

// Propagates error from a function call.
#define E_PROP(call)                                                           \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUC)                                                   \
            return _errno;                                                     \
    } while (0)

// Handles an error with `goto` and sets the `ret_val` to the error code.
#define E_HANDLE(call, ret_val, label)                                         \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUC) {                                                 \
            ret_val = _errno;                                                  \
            goto label;                                                        \
        }                                                                      \
    } while (0)
