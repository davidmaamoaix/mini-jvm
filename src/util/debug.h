#ifndef MINIJVM_DEBUG_H
#define MINIJVM_DEBUG_H

#ifndef DEBUG
#define VERBOSE(format, args...) do { printf(format, ##args); } while (0)
#else
#define VERBOSE(...)
#endif

#endif //MINIJVM_DEBUG_H
