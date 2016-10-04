#if !defined(MTB_HEADER_mtb_platform)
#define MTB_HEADER_mtb_platform

// MTB_On and MTB_Off are used to ensure an option was defined before it was
// used. MTB_IsOn is used with something undefined

//
// Usage:
//   #define MY_SETTING MTB_On
//   #if MTB_IsOn(MY_SETTING)
//     /* Do stuff */
//   #endif
#define MTB_On  +
#define MTB_Off -
#define MTB_IsOn(x)  ((1 x 1) == 2)
#define MTB_IsOff(x) (!MTB_IsOn(x))


#if defined(__cplusplus)
  #define MTB_Cpp MTB_On
#else
  #define MTB_Cpp MTB_Off
#endif

// NOTE: Only c++ enabled for now.
#if MTB_IsOff(MTB_Cpp)
  #error MTB requires C++ (for now).
#endif


//
// Determine the compiler
// MTB_Compiler_*
//

// Usage:
//   #if MTB_IsCompiler(MSVC)
//   /* ... */
//   #endif
#define MTB_IsCompiler(WHAT) MTB_IsOn(MTB_Compiler_##WHAT)

#define MTB_Compiler_MSVC MTB_Off
#define MTB_Compiler_GCC MTB_Off
#define MTB_Compiler_Clang MTB_Off

#if defined(_MSC_VER)
  #undef  MTB_Compiler_MSVC
  #define MTB_Compiler_MSVC MTB_On
#else
  #error "Unknown compiler."
#endif


//
// Determine the operating system
// MTB_OS_*
//

#define MTB_OS_Windows MTB_Off

#if defined(_WIN32) || defined(_WIN64)
  #undef  MTB_OS_Windows
  #define MTB_OS_Windows MTB_On
#else
  // TODO: Emit some kind of warning instead?
  #error "Unknown operating system."
#endif

#define MTB_IsOS(WHAT) MTB_IsOn(MTB_OS_##WHAT)


//
// Determine the architecture and bitness
// MTB_Arch_*
//

// Usage:
// #if MTB_IsArch(x86)
// /* ... */
// #endif
#define MTB_IsArch(WHAT) MTB_IsOn(MTB_Arch_##WHAT)

#define MTB_Arch_x86    MTB_Off /* 32 bit version of the Intel x86 architecture. */
#define MTB_Arch_x86_64 MTB_Off /* 64 bit version of the Intel x86 architecture. */

#if defined(_M_X64) || defined(_M_AMD64)
  #undef  MTB_Arch_x86_64
  #define MTB_Arch_x86_64 MTB_On
#elif defined(_M_IX86) || defined(_M_I86)
  #undef  MTB_Arch_x86
  #define MTB_Arch_x86 MTB_On
#else
  #error "Unknown architecture."
#endif

#define MTB_32BitArch MTB_IsOff
#define MTB_64BitArch MTB_IsOff
#define MTB_LittleEndian MTB_IsOff
#define MTB_BigEndian MTB_IsOff

#if MTB_IsArch(x86_64)
  #undef  MTB_64BitArch
  #define MTB_64BitArch MTB_On
  #undef  MTB_LittleEndian
  #define MTB_LittleEndian MTB_On
#elif MTB_IsArch(x86)
  #undef  MTB_32BitArch
  #define MTB_32BitArch MTB_On
  #undef  MTB_LittleEndian
  #define MTB_LittleEndian MTB_On
#else
  #error "Undefined architecture."
#endif


//
// Utility stuff
//

#define MTB_File __FILE__
#define MTB_Line __LINE__

#if defined(__PRETTY_FUNCTION__)
  #define MTB_Function __PRETTY_FUNCTION__
#elif defined(__func__)
  #define MTB_Function __func__
#elif defined(__FUNCTION__)
  #define MTB_Function __FUNCTION__
#else
  #define MTB_Function ""
#endif

#define MTB_Impl_ConcatLevel2(LEFT, RIGHT) LEFT##RIGHT
#define MTB_Impl_ConcatLevel1(LEFT, RIGHT) MTB_Impl_ConcatLevel2(LEFT, RIGHT)
#define MTB_Concat(LEFT, RIGHT) MTB_Impl_ConcatLevel1(LEFT, RIGHT)

#define MTB_Impl_Stringify(WHAT) #WHAT
#define MTB_Stringify(WHAT) MTB_Impl_Stringify(WHAT)

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
//   MTB_Inline void func() { /* ... */ }
#if !defined(MTB_Inline)
  #if MTB_IsCompiler(MSVC)
    #define MTB_Inline __forceinline
  #else
    // TODO: Emit a warning here?
    // Fallback to `inline`
    #define MTB_Inline inline
  #endif
#endif


// Try to automatically determine a debug build if the user did not specify anything.
#if !defined(MTB_DebugBuild)
  #if defined(_DEBUG)
    #define MTB_DebugBuild MTB_On
  #else
    #define MTB_DebugBuild MTB_Off
  #endif
#endif

// Try to automatically determine a release build if the user did not specify anything.
#if !defined(MTB_ReleaseBuild)
  #if defined(NDEBUG)
    #define MTB_ReleaseBuild MTB_On
  #else
    #define MTB_ReleaseBuild MTB_Off
  #endif
#endif

// Try to determine a development build if the user did not specify anything.
#if !defined(MTB_DevBuild)
  #if defined(MTB_ReleaseBuild)
    #define MTB_DevBuild MTB_Off
  #else
    #define MTB_DevBuild MTB_On
  #endif
#endif

#if !defined(MTB_Internal)
  #if defined(MTB_ReleaseBuild)
    #define MTB_Internal MTB_Off
  #else
    #define MTB_Internal MTB_On
  #endif
#endif

// Macro to enclose code that is only compiled in in debug builds.
//
// Usage:
//   MTB_DebugCode(auto result = ) someFunctionCall();
//   MTB_DebugCode(if(result == 0))
#if !defined(MTB_DebugCode)
  #if MTB_IsOn(MTB_DebugBuild)
    #define MTB_DebugCode(...) __VA_ARGS__
  #else
    #define MTB_DebugCode(...)
  #endif
#endif

#if !defined(MTB_DebugBreak)
  #if MTB_IsCompiler(MSVC)
    #define MTB_DebugBreak do { __debugbreak(); } while(0)
  #else
    #define MTB_DebugBreak MTB_NOP
  #endif
#endif

#if !defined(MTB_Exceptions)
  #define MTB_Exceptions MTB_Off
#endif

// For STL
// NOTE: Don't undef to inform the user that we're overwriting their settings
// if they specified it.
#if MTB_IsOn(MTB_Exceptions)
  #define _HAS_EXCEPTIONS 1
#else
  #define _HAS_EXCEPTIONS 0
#endif


//
// Primitive types.
//
namespace mtb
{
  // NOTE: These are the only types in MTB that are not prefixed.
  using i8  = char;
  using i16 = short;
  using i32 = int;
  using i64 = long long;

  using u8  = unsigned char;
  using u16 = unsigned short;
  using u32 = unsigned int;
  using u64 = unsigned long long;

  using f32 = float;
  using f64 = double;

  using bool32 = i32;

  /// Generic types if no specific size/precision is required.
  using uint = unsigned int;
} // namespace mtb

#endif // !defined(MTB_HEADER_mtb_platform)
