#pragma once

// Somewhat similar to C's error codes, but actually quite different.
typedef enum {
    E_SUCC = 0,  // No error.
    E_PERM = 1,  // No permission.
    E_NOFD = 2,  // No such file or directory.
    E_EOFL = 3,  // End of file.
    E_IOER = 5,  // IO Error.
    E_NMEM = 12, // Out of memory.
    E_IBTC = 13, // Invalid bytecode.
    E_ICFE = 14, // Corrupted class file.
    E_UTFE = 15  // UTF-8 decoding error.
} err_vm;

// Checks memory allocation with error propagation.
#define E_MEM_PROP(val)                                                        \
    if (val == NULL)                                                           \
        return E_NMEM;

// Checls memory allocation and handles it with `goto` and sets `ret_val`.
#define E_MEM_HANDLE(val, ret_val, label)                                      \
    if (val == NULL) {                                                         \
        ret_val = E_NMEM;                                                      \
        goto label;                                                            \
    }

// Propagates error from a function call.
#define E_PROP(call)                                                           \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUCC)                                                  \
            return _errno;                                                     \
    } while (0)

// Handles an error with `goto` and sets the `ret_val` to the error code.
#define E_HANDLE(call, ret_val, label)                                         \
    do {                                                                       \
        err_vm _errno = (call);                                                \
        if (_errno != E_SUCC) {                                                \
            ret_val = _errno;                                                  \
            goto label;                                                        \
        }                                                                      \
    } while (0)

// Asserts and propagates.
#define E_ASSERT_PROP(exp, err)                                                \
    do {                                                                       \
        if (!(exp))                                                            \
            return err;                                                        \
    } while (0)

// Asserts and handles.
#define E_ASSERT_HANDLE(exp, err, ret_val, label)                              \
    do {                                                                       \
        if (!(exp)) {                                                          \
            ret_val = err;                                                     \
            goto label;                                                        \
        }                                                                      \
    } while (0)
