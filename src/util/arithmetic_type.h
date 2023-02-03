#ifndef KANON_UTIL_ARITHMETIC_H
#define KANON_UTIL_ARITHMETIC_H

#include <cstdint>
#include <cstdlib>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intmax_t imax;
typedef uintptr_t iptr;

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long ullong;

#ifdef _WIN32
typedef long long isize;
#else defined(__unix__)
typedef ssize_t isize;
#endif

typedef size_t usize;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintmax_t umax;

typedef float f32;
typedef double f64;
typedef long double lf64; // FIXME f96?

#endif // KANON_UTIL_ARITHMETIC_H
