#ifndef MINIJVM_CLASSLOADER_H
#define MINIJVM_CLASSLOADER_H

#include <inttypes.h>

#include "lang/class.h"
#include "lang/class_member.h"
#include "util/bytestream.h"

ByteStream *readClass(const char *);
Class *loadClass(const char *);
void freeClass(Class *);

Constant *readConstPool(ByteStream *, uint16_t);
void freeConstPool(Constant *, uint16_t);
Interface *readInterfaces(ByteStream *, uint16_t);

Field *readFields(ByteStream *, uint16_t);
void freeFields(Field *, uint16_t);
Attribute *readAttrs(ByteStream *, uint16_t);
void freeAttrs(Attribute *, uint16_t);

Method *readMethods(ByteStream *, uint16_t);
void freeMethods(Method *, uint16_t);

// some non-definitive verification
uint8_t verifyClass(Class *);

// parse code of method attrs
void parseMethodCode();

#endif //MINIJVM_CLASSLOADER_H
