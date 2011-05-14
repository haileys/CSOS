#ifndef STDINT_H
#define STDINT_H

typedef signed char int8_t;
typedef unsigned char uint8_t;

#define INT8_MAX (127)
#define INT8_MIN (-128)
#define UINT8_MAX (255)

typedef signed short int16_t;
typedef unsigned short uint16_t;

#define INT16_MAX (32767)
#define INT16_MIN (-32768)
#define UINT16_MAX (65536)

typedef signed int int32_t;
typedef unsigned int uint32_t;

#define INT32_MAX (2147483647)
#define INT32_MIN (-2147483648)
#define UINT32_MAX (4294967296u)
#define INT_MAX INT32_MAX
#define INT_MIN INT32_MIN
#define UINT_MAX UINT32_MAX

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

#define INT64_MAX (1ul << 63 - 1ul)
#define INT64_MIN (~0ul)
#define UINT64_MAX (0xfffffffffffffffful)

#endif