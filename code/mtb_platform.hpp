#if !defined(MTB_HEADER_mtb_platform)
#define MTB_HEADER_mtb_platform

// NOTE: Only c++ enabled for now.
#define MTB_CPP __cplusplus


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
  #define MTB_IS_LITTLE_ENDIAN 1
  #define MTB_IS_BIG_ENDIAN 0
#elif MTB_CURRENT_ARCH == MTB_ARCH_TYPE_x86
  #define MTB_CURRENT_ARCH_BITNESS 32
  #define MTB_IS_LITTLE_ENDIAN 1
  #define MTB_IS_BIG_ENDIAN 0
#else
  #define MTB_CURRENT_ARCH_BITNESS 0
  #define MTB_IS_LITTLE_ENDIAN 0
  #define MTB_IS_BIG_ENDIAN 0
  #error "Undefined architecture."
#endif

#if !defined(MTB_CURRENT_ARCH_BITNESS) || !defined(MTB_IS_LITTLE_ENDIAN) || !defined(MTB_IS_BIG_ENDIAN)
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

#define MTB_IMPL_STRINGIFY(WHAT) #WHAT
#define MTB_STRINGIFY(WHAT) MTB_IMPL_STRINGIFY(WHAT)

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
// Determine the current type of build
//

#define MTB_BUILD_TYPE_UNKNOWN 0
#define MTB_BUILD_TYPE_DEBUG   10
#define MTB_BUILD_TYPE_DEV     20
#define MTB_BUILD_TYPE_RELEASE 30

#if defined(MTB_CURRENT_BUILD)
  #error MTB_CURRENT_BUILD must not be set manually.
#endif


//
// Handle manual MTB_IS_*_BUILD setting.
//

#if defined(MTB_IS_DEBUG_BUILD) || defined(MTB_IS_DEV_BUILD) || defined(MTB_IS_RELEASE_BUILD)
  #if MTB_IS_DEBUG_BUILD
    #define MTB_CURRENT_BUILD MTB_BUILD_TYPE_DEBUG
  #elif MTB_IS_DEV_BUILD
    #define MTB_CURRENT_BUILD MTB_BUILD_TYPE_DEV
    #define MTB_CURRENT_BUILD MTB_BUILD_TYPE_RELEASE
  #elif
    #define MTB_CURRENT_BUILD MTB_BUILD_TYPE_UNKNOWN
  #endif
#endif


//
// Try to automatically determine the current build type if non is set at this point.
//

#if !defined(MTB_CURRENT_BUILD)
  #if defined(_DEBUG) || defined(DEBUG)
  #elif defined(NDEBUG)
  #else
  #endif
#endif


//
// Determine the MTB_IS_*_BUILD settings if they're not already set.
//

#if !defined(MTB_IS_DEBUG_BUILD)
  #if MTB_CURRENT_BUILD == MTB_BUILD_TYPE_DEBUG
    #define MTB_IS_DEBUG_BUILD 1
  #else
    #define MTB_IS_DEBUG_BUILD 0
  #endif
#endif

#if !defined(MTB_IS_DEV_BUILD)
  #if MTB_CURRENT_BUILD == MTB_BUILD_TYPE_DEV
    #define MTB_IS_DEV_BUILD 1
  #else
    #define MTB_IS_DEV_BUILD 0
  #endif
#endif

#if !defined(MTB_IS_RELEASE_BUILD)
  #if MTB_CURRENT_BUILD == MTB_BUILD_TYPE_RELEASE
    #define MTB_IS_RELEASE_BUILD 1
  #else
    #define MTB_IS_RELEASE_BUILD 0
  #endif
#endif

#if !defined(MTB_IS_INTERNAL_BUILD)
  #if MTB_CURRENT_BUILD < MTB_BUILD_TYPE_RELEASE
    #define MTB_IS_INTERNAL_BUILD 1
  #else
    #define MTB_IS_INTERNAL_BUILD 0
  #endif
#endif

// Validate settings
#if (MTB_IS_DEBUG_BUILD + MTB_IS_DEV_BUILD + MTB_IS_RELEASE_BUILD) > 1
  // TODO(Manuzor): Should the MTB_IS_*_BUILD settings really be mutually exclusive?
  #error Only one of these may be set at a time: MTB_IS_DEBUG_BUILD, MTB_IS_DEV_BUILD, MTB_IS_RELEASE_BUILD
#endif

// Macro to enclose code that is only compiled in in the corresponding build type.
//
// Usage:
//   MTB_DEBUG_CODE(auto result = ) someFunctionCall();
//   MTB_DEBUG_CODE(if(result == 0) { /* ... */ })
#if !defined(MTB_DEBUG_CODE)
  #if MTB_CURRENT_BUILD <= MTB_BUILD_TYPE_DEBUG
    #define MTB_DEBUG_CODE(...) __VA_ARGS__
  #else
    #define MTB_DEBUG_CODE(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_DEV_CODE)
  #if MTB_CURRENT_BUILD <= MTB_BUILD_TYPE_DEV
    #define MTB_DEV_CODE(...) __VA_ARGS__
  #else
    #define MTB_DEV_CODE(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_RELEASE_CODE)
  #if MTB_CURRENT_BUILD <= MTB_BUILD_TYPE_RELEASE
    #define MTB_RELEASE_CODE(...) __VA_ARGS__
  #else
    #define MTB_RELEASE_CODE(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_INTERNAL_CODE)
  #if MTB_CURRENT_BUILD <= MTB_BUILD_TYPE_INTERNAL
    #define MTB_INTERNAL_CODE(...) __VA_ARGS__
  #else
    #define MTB_INTERNAL_CODE(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_DebugBreak)
  #if MTB_CURRENT_COMPILER == MTB_COMPILER_TYPE_MSVC
    #define MTB_DebugBreak() do { __debugbreak(); } while(0)
  #else
    #define MTB_DebugBreak() MTB_NOP
  #endif
#endif

#if !defined(MTB_HAS_EXCEPTIONS)
  #define MTB_HAS_EXCEPTIONS 0
#endif

// For STL
// NOTE: Don't undef to inform the user that we're overwriting their settings
// if they specified it.
#if !defined(_HAS_EXCEPTIONS)
  #if MTB_HAS_EXCEPTIONS
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
