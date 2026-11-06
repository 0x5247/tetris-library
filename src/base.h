#pragma once

typedef _Bool bool;
#define true 1
#define false 0

typedef __SIZE_TYPE__ size_t;
typedef __INT32_TYPE__ ssize_t;

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INTPTR_TYPE__ intptr_t;

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINTPTR_TYPE__ uintptr_t;

#define ABS(VAL) ((VAL) < 0 ? -(VAL) : (VAL))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
