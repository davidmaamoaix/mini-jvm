#pragma once

// Field `access_flags`.
#define F_ACC_PUBLIC                0x0001
#define F_ACC_FINAL                 0x0010
#define F_ACC_SUPER                 0x0020
#define F_ACC_INTERFACE             0x0200
#define F_ACC_ABSTRACT              0x0400
#define F_ACC_SYNTHETIC             0x1000
#define F_ACC_ANNOTATION            0x2000
#define F_ACC_ENUM                  0x4000

// Method `access_flags`.
#define M_ACC_PUBLIC                0x0001
#define M_ACC_PRIVATE               0x0002
#define M_ACC_PROTECTED             0x0004
#define M_ACC_STATIC                0x0008
#define M_ACC_FINAL                 0x0010
#define M_ACC_SYNCHRONIZED          0x0020
#define M_ACC_BRIDGE                0x0040
#define M_ACC_VARARGS               0x0080
#define M_ACC_NATIVE                0x0100
#define M_ACC_ABSTRACT              0x0400
#define M_ACC_STRICT                0x0800
#define M_ACC_SYNTHETIC             0x1000

// Constant pool types.
#define CONSTANT_Utf8               1
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_Class              7
#define CONSTANT_String             8
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType        12
#define CONSTANT_MethodHandle       15
#define CONSTANT_MethodType         16
#define CONSTANT_InvokeDynamic      18
