#ifndef KURO_UTIL_H__
#define KURO_UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef likely 
#define likely __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif 

#define ASSERT(cond) \
  ({if (unlikely((cond) == 0)) \
   {\
    char buf[1536]; \
    ::memset(buf, 0, sizeof buf); \
    ::snprintf(buf, sizeof buf, "Aborted.\nReason: %s %s() - %s:%d\n", \
               #cond, __func__, __FILE__, __LINE__) ; \
    ::fputs(buf, stderr); \
    ::fflush(stderr); \
    ::abort();\
   }})

#endif
