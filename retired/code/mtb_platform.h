#if !defined(MTB_HEADER_mtb_platform)
#define MTB_HEADER_mtb_platform

// NOTE: Only c++ enabled for now.
#define MTB_CPP __cplusplus


//
// Clean preprocessor flags
//

#define MTB_ON +
#define MTB_OFF -

// Usage:
//   #define MTB_FLAG_FOO MTB_ON
//   #if MTB_FLAG(FOO) /* ... */
#define MTB_FLAG(OP) (!!(1 MTB_FLAG_##OP 1))


//
// MTB_CURRENT_COMPILER
// Determine the compiler
//
#define MTB_COMPILER_TYPE_UNKNOWN  0
#define MTB_COMPILER_TYPE_MSVC    10
#define MTB_COMPILER_TYPE_GCC     20
#define MTB_COMPILER_TYPE_CLANG   30

#if defined(_MSC_VER)
  #define MTB_CURRENT_COMPILER MTB_COMPILER_TYPE_MSVC
#else
  #define MTB_CURRENT_COMPILER MTB_COMPILER_TYPE_UNKNOWN
  #error "Unknown compiler."
#endif


//
// MTB_CURRENT_OS
// Determine the operating system
//
#define MTB_OS_TYPE_UNKNOWN  0
#define MTB_OS_TYPE_WINDOWS 10
#define MTB_OS_TYPE_LINUX   20
#define MTB_OS_TYPE_OSX     30


#if defined(_WIN32) || defined(_WIN64)
  #define MTB_CURRENT_OS MTB_OS_TYPE_WINDOWS
#else
  // TODO: Emit some kind of warning instead?
  #error "Unknown operating system."
  #define MTB_CURRENT_OS MTB_OS_TYPE_UNKNOWN
#endif


//
// MTB_CURRENT_ARCH
// Determine the architecture and bitness
//
#define MTB_ARCH_TYPE_UNKNOWN  0
#define MTB_ARCH_TYPE_x86     10
#define MTB_ARCH_TYPE_x64     20 // aka x86_64, amd64

#if defined(_M_X64) || defined(_M_AMD64)
  #define MTB_CURRENT_ARCH MTB_ARCH_TYPE_x64
#elif defined(_M_IX86) || defined(_M_I86)
  #define MTB_CURRENT_ARCH MTB_ARCH_TYPE_x86
#else
  #define MTB_CURRENT_ARCH MTB_ARCH_TYPE_UNKNOWN
  #error "Unknown architecture."
#endif

#if MTB_CURRENT_ARCH == MTB_ARCH_TYPE_x64
  #define MTB_CURRENT_ARCH_BITNESS 64
  #define MTB_FLAG_LITTLE_ENDIAN MTB_ON
  #define MTB_FLAG_BIG_ENDIAN MTB_OFF
#elif MTB_CURRENT_ARCH == MTB_ARCH_TYPE_x86
  #define MTB_CURRENT_ARCH_BITNESS 32
  #define MTB_FLAG_LITTLE_ENDIAN MTB_ON
  #define MTB_FLAG_BIG_ENDIAN MTB_OFF
#else
  #define MTB_CURRENT_ARCH_BITNESS 0
  #define MTB_FLAG_LITTLE_ENDIAN MTB_OFF
  #define MTB_FLAG_BIG_ENDIAN MTB_OFF
  #error "Undefined architecture."
#endif

#if !defined(MTB_CURRENT_ARCH_BITNESS) || !defined(MTB_FLAG_LITTLE_ENDIAN) || !defined(MTB_FLAG_BIG_ENDIAN)
  #error Invalid settings.
#endif


//
// Utility stuff
//

#define MTB_CURRENT_FILE __FILE__
#define MTB_CURRENT_LINE __LINE__

#if defined(__PRETTY_FUNCTION__)
  #define MTB_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__func__)
  #define MTB_CURRENT_FUNCTION __func__
#elif defined(__FUNCTION__)
  #define MTB_CURRENT_FUNCTION __FUNCTION__
#else
  #define MTB_CURRENT_FUNCTION ""
#endif

#define MTB_IMPL_CONCAT_L2(LEFT, RIGHT) LEFT##RIGHT
#define MTB_IMPL_CONCAT_L1(LEFT, RIGHT) MTB_IMPL_CONCAT_L2(LEFT, RIGHT)
#define MTB_CONCAT(LEFT, RIGHT) MTB_IMPL_CONCAT_L1(LEFT, RIGHT)

#define MTB_IMPL_STRINGIFY_1(WHAT) #WHAT
#define MTB_IMPL_STRINGIFY_0(WHAT) MTB_IMPL_STRINGIFY_1(WHAT)
#define MTB_STRINGIFY(WHAT) MTB_IMPL_STRINGIFY_0(WHAT)

// A no-op that can be safely terminated with a semicolon.
#if !defined(MTB_NOP)
  #define MTB_NOP do{}while(0)
#endif


// Define a macro that has the strongest inline directive for the current
// compiler.
//
// If you need only a hint for the compiler, use the keyword `inline`
// directly.
//
// Usage:
//   MTB_INLINE void func() { /* ... */ }
#if !defined(MTB_INLINE)
  #if MTB_CURRENT_COMPILER == MTB_COMPILER_TYPE_MSVC
    #define MTB_INLINE __forceinline
  #else
    // TODO: Emit a warning here?
    // Fallback to `inline`
    #define MTB_INLINE inline
  #endif
#endif

//
// Determine build flags.
//
#if !defined(MTB_FLAG_RELEASE)
  #if defined(NDEBUG)
    #define MTB_FLAG_RELEASE MTB_ON
  #else
    #define MTB_FLAG_RELEASE MTB_OFF
  #endif
#endif

#if !defined(MTB_FLAG_DEBUG)
  #if defined(_DEBUG) || defined(DEBUG) || !MTB_FLAG(RELEASE)
    #define MTB_FLAG_DEBUG MTB_ON
  #else
    #define MTB_FLAG_DEBUG MTB_OFF
  #endif
#endif

#if !defined(MTB_FLAG_INTERNAL)
  #if !MTB_FLAG(RELEASE)
    #define MTB_FLAG_INTERNAL MTB_ON
  #else
    #define MTB_FLAG_INTERNAL MTB_OFF
  #endif
#endif

#if !defined(MTB_FLAG_BOUNDS_CHECKING)
  #if !MTB_FLAG(RELEASE)
    #define MTB_FLAG_BOUNDS_CHECKING MTB_ON
  #else
    #define MTB_FLAG_BOUNDS_CHECKING MTB_OFF
  #endif
#endif

// Validate settings
#if !(MTB_FLAG(DEBUG) + MTB_FLAG(RELEASE) == 1)
  #pragma message("DEBUG: " MTB_STRINGIFY(MTB_FLAG(DEBUG)))
  #pragma message("RELEASE: " MTB_STRINGIFY(MTB_FLAG(RELEASE)))
  #error Only one of these may be set at a time: MTB_FLAG_DEBUG, MTB_FLAG_DEV, MTB_FLAG_RELEASE
#endif

// Macro to enclose code that is only compiled in in the corresponding build type.
//
// Usage:
//   MTB_DEBUG_CODE(auto result = ) someFunctionCall();
//   MTB_DEBUG_CODE(if(result == 0) { /* ... */ })
#if !defined(MTB_DEBUG_CODE)
  #if MTB_FLAG(DEBUG)
    #define MTB_DEBUG_CODE(...) __VA_ARGS__
  #else
    #define MTB_DEBUG_CODE(...)
  #endif
#endif

#if !defined(MTB_INTERNAL_CODE)
  #if MTB_FLAG(INTERNAL)
    #define MTB_INTERNAL_CODE(...) __VA_ARGS__
  #else
    #define MTB_INTERNAL_CODE(...)
  #endif
#endif

#if !defined(MTB_DebugBreak)
  #if MTB_CURRENT_COMPILER == MTB_COMPILER_TYPE_MSVC
    #define MTB_DebugBreak() do { __debugbreak(); } while(0)
  #else
    #define MTB_DebugBreak() MTB_NOP
  #endif
#endif

#if !defined(MTB_FLAG_HAS_EXCEPTIONS)
  #define MTB_FLAG_HAS_EXCEPTIONS MTB_OFF
#endif

// For STL
// NOTE: Don't undef to inform the user that we're overwriting their settings
// if they specified it.
#if !defined(_HAS_EXCEPTIONS)
  #if MTB_FLAG(HAS_EXCEPTIONS)
    #define _HAS_EXCEPTIONS 1
  #else
    #define _HAS_EXCEPTIONS 0
  #endif
#endif


//
// Primitive types.
//
// TODO: Multiple platforms.

using mtb_int = signed int;
using mtb_uint = unsigned int;

using mtb_s08 = signed char;
using mtb_s16 = signed short;
using mtb_s32 = signed int;
using mtb_s64 = signed long long;

using mtb_u08 = unsigned char;
using mtb_u16 = unsigned short;
using mtb_u32 = unsigned int;
using mtb_u64 = unsigned long long;

using mtb_f32 = float;
using mtb_f64 = double;

using mtb_byte = mtb_u08;


#endif // !defined(MTB_HEADER_mtb_platform)
