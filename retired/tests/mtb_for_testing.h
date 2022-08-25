#define MTB_TESTS 1
#define MTB_ASSERT(...)                 \
    do                                  \
    {                                   \
        if(!(__VA_ARGS__))              \
        {                               \
            DOCTEST_FAIL(#__VA_ARGS__); \
        }                               \
    } while(false)

#include "..\mtb.h"
#include "..\mtb_rng.h"
