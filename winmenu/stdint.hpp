/**
 * @author    Dmitry Selyutin
 * @copyright GNU General Public License v3.0+
 */

#ifndef WINAPPUSAGE_STDINT_HPP
#define WINAPPUSAGE_STDINT_HPP
#include "config.hpp"


// Check for stdint.h
#if (defined(__STDC__) && (__STDC__) && (__STDC_VERSION__ >= 199901L)) \
|| (defined (__WATCOMC__) && (__WATCOMC__ >= 1250)) \
|| (defined(__GNUC__))
  #include <stdint.h>
#endif


// Maximum size
#if !defined(SIZE_MAX)
  #define SIZE_MAX (~(size_t)0)
#endif


// 8-bit integer
#if ((SCHAR_MIN == -128) && (SCHAR_MAX == 127))
  #if !defined(INT8_MIN)
    #define INT8_MIN SCHAR_MIN
  #endif
  #if !defined(INT8_MAX)
    #define INT8_MAX SCHAR_MAX
  #endif
typedef signed char int8_t;
#else
  #error Cannot determine 8-bit signed int type
#endif

#if (UCHAR_MAX == 255)
  #if !defined(UINT8_MAX)
    #define UINT8_MAX UCHAR_MAX
  #endif
typedef unsigned char uint8_t;
#else
  #error Cannot determine 8-bit unsigned int type
#endif


// 16-bit integer
#if ((SHRT_MIN == -32768) && (SHRT_MAX == 32767))
  #if !defined(INT16_MIN)
    #define INT16_MIN SHRT_MIN
  #endif
  #if !defined(INT16_MAX)
    #define INT16_MAX SHRT_MAX
  #endif
typedef signed short int16_t;
#elif ((INT_MIN == -32768) && (INT_MAX == 32767))
  #if !defined(INT16_MIN)
    #define INT16_MIN INT_MIN
  #endif
  #if !defined(INT16_MAX)
    #define INT16_MAX INT_MAX
  #endif
typedef signed int int16_t;
#else
  #error Cannot determine 16-bit signed int type
#endif

#if (USHRT_MAX == 65535)
  #if !defined(UINT16_MAX)
    #define UINT16_MAX USHRT_MAX
  #endif
typedef unsigned short uint16_t;
#elif (UINT_MAX == 65535)
  #if !defined(UINT16_MAX)
    #define UINT16_MAX UINT_MAX
  #endif
typedef unsigned int uint16_t;
#else
  #error Cannot determine 16-bit unsigned int type
#endif


// 32-bit integer
#if ((INT_MIN == -2147483648) && (INT_MAX == 2147483647))
  #if !defined(INT32_MIN)
    #define INT32_MIN INT_MIN
  #endif
  #if !defined(INT32_MAX)
    #define INT32_MAX INT_MAX
  #endif
typedef signed int int32_t;
#elif ((LONG_MIN == -2147483648) && (LONG_MAX == 2147483647))
  #if !defined(INT32_MIN)
    #define INT32_MIN LONG_MIN
  #endif
  #if !defined(INT32_MAX)
    #define INT32_MAX LONG_MAX
  #endif
typedef signed long int32_t;
#else
  #error Cannot determine 32-bit signed int type
#endif

#if (UINT_MAX == 4294967295)
  #if !defined(UINT32_MAX)
    #define UINT32_MAX UINT_MAX
  #endif
typedef unsigned int uint32_t;
#elif (ULONG_MAX == 4294967295)
  #if !defined(UINT32_MAX)
    #define UINT32_MAX ULONG_MAX
  #endif
typedef unsigned long uint32_t;
#else
  #error Cannot determine 32-bit unsigned int type
#endif


// 64-bit integer
#if defined(_POSIX_V6_ILP32_OFFBIG) \
|| defined(_POSIX_V6_LPBIG_OFFBIG) \
|| defined(_POSIX_V6_LP64_OFF64) \
|| (defined(__WATCOMC__) && defined(__WATCOM_INT64__)) \
|| (defined(_MSC_VER) && (_INTEGRAL_MAX_BITS >= 64)) \
|| (defined(__BORLANDC__) && (__BORLANDC__ > 0x460)) \
|| defined(__GNUC__) \
|| defined(__MWERKS__) \
|| defined(__SUNPRO_C) \
|| defined(__SUNPRO_CC) \
|| defined(__APPLE_CC__) \
|| defined(__alpha) \
|| defined(__DECC) \
|| defined(_CRAYC) \
|| defined(_LONG_LONG) \
|| defined(ULLONG_MAX)
  #if defined(__GNUC__)
    #if (__WORDSIZE == 64)
      #if !defined(INT64_MIN)
        #define INT64_MIN -9223372036854775808L
      #endif
      #if !defined(INT64_MAX)
        #define INT64_MAX 9223372036854775807L
      #endif
      #if !defined(UINT64_MAX)
        #define UINT64_MAX 18446744073709551615UL
      #endif
typedef signed long int int64_t;
typedef unsigned long int uint64_t;
    #else
      #if !defined(INT64_MIN)
        #define INT64_MIN -9223372036854775808LL
      #endif
      #if !defined(INT64_MAX)
        #define INT64_MAX 9223372036854775807LL
      #endif
      #if !defined(UINT64_MAX)
        #define UINT64_MAX 18446744073709551615ULL
      #endif
__extension__ typedef signed long long int64_t;
__extension__ typedef unsigned long long uint64_t;
    #endif

  #elif defined(__MWERKS__) \
  || defined (__SUNPRO_C) \
  || defined (__SUNPRO_CC) \
  || defined (__APPLE_CC__) \
  || defined (_LONG_LONG) \
  || defined (_CRAYC)
    #if !defined(INT64_MIN)
      #define INT64_MIN -9223372036854775808LL
    #endif
    #if !defined(INT64_MAX)
      #define INT64_MAX 9223372036854775807LL
    #endif
    #if !defined(UINT64_MAX)
      #define UINT64_MAX 18446744073709551615ULL
    #endif
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

  #elif (defined(__WATCOMC__) && defined(__WATCOM_INT64__)) \
  || (defined(_MSC_VER) && (_INTEGRAL_MAX_BITS >= 64)) \
  || (defined(__BORLANDC__) && (__BORLANDC__ > 0x460)) \
  || defined(__alpha) \
  || defined(__DECC)
    #if !defined(INT64_MIN)
      #define INT64_MIN -9223372036854775808LL
    #endif
    #if !defined(INT64_MAX)
      #define INT64_MAX 9223372036854775807LL
    #endif
    #if !defined(UINT64_MAX)
      #define UINT64_MAX 18446744073709551615ULL
    #endif
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
  #endif
#endif


// Largest integer
#if defined(UINT64_MAX)
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;
#else
typedef int32_t intmax_t;
typedef uint32_t uintmax_t;
#endif


// Maximum size
#if !defined(SIZE_MAX)
  #define SIZE_MAX UINT64_MAX
#endif


// Byte type
typedef uint8_t byte;


// Byte sets
union byte16set
{
  uint16_t code;
  uint8_t bytes[2];
};

union byte32set
{
  uint32_t code;
  uint8_t bytes[4];
};

union byte64set
{
  uint32_t code;
  uint8_t bytes[8];
};


#endif // WINAPPUSAGE_STDINT_HPP
