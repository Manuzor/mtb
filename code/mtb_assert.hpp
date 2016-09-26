#if !defined(MTB_HEADER_mtb_assert)
#define MTB_HEADER_mtb_assert

#include "mtb.hpp"

namespace mtb
{
/// Returning \c true means that the a debug break is required.
using AssertHandler = bool(*)(
  char const* FileName,     //< The name of the file where the assert is.
  size_t      Line,         //< The line number in the file where the assert is.
  char const* FunctionName, //< The name of the function where the assert is.
  char const* Expression,   //< The condition as captured by the MTB_Assert macro.
  char const* Message);     //< The user defined message.

AssertHandler GetAssertHandler();
void SetAssertHandler(AssertHandler AssertHandler);

bool OnFailedCheck(
  char const* FileName,
  size_t      Line,
  char const* FunctionName,
  char const* Expression,
  char const* MessageFormat = nullptr,
  ...);


//
// Define the actual assertion macro.
//
#if !defined(MTB_Require)
  #define MTB_Require(CONDITION, ...) \
    do \
    { \
      if(!(CONDITION)) \
      { \
        if(mtb_OnFailedCheck(MTB_File, MTB_Line, MTB_Function, #CONDITION, __VA_ARGS__)) \
        { \
          MTB_DebugBreak; \
        } \
      } \
    } while(0)
#endif


//
// Find sensible defaults if the user did not set anything.
//
#if !defined(MTB_WantDebugAssert)
  #if MTB_IsOn(MTB_DebugBuild)
    #define MTB_WantDebugAssert MTB_On
  #else
    #define MTB_WantDebugAssert MTB_Off
  #endif
#endif

#if !defined(MTB_WantDevAssert)
  #if MTB_IsOn(MTB_DevBuild)
    #define MTB_WantDevAssert MTB_On
  #else
    #define MTB_WantDevAssert MTB_Off
  #endif
#endif

#if !defined(MTB_WantReleaseAssert)
  #if MTB_IsOn(MTB_ReleaseBuild)
    #define MTB_WantReleaseAssert MTB_On
  #else
    #define MTB_WantReleaseAssert MTB_Off
  #endif
#endif


//
// Define the actual assert macros
//
#if MTB_IsOn(MTB_WantDebugAssert)
  #define MTB_DebugAssert MTB_Require
#endif

#if MTB_IsOn(MTB_WantDevAssert)
  #define MTB_DevAssert MTB_Require
#endif

#if MTB_IsOn(MTB_WantReleaseAssert)
  #define MTB_ReleaseAssert MTB_Require
#endif


//
// Make sure all assert macros are at least defined.
//
#if !defined(MTB_DebugAssert)
  #define MTB_DebugAssert(...) MTB_NOP
#endif

#if !defined(MTB_DevAssert)
  #define MTB_DevAssert(...) MTB_NOP
#endif

#if !defined(MTB_ReleaseAssert)
  #define MTB_ReleaseAssert(...) MTB_NOP
#endif

} // namespace mtb
#endif // !defined(MTB_HEADER_mtb_assert)


// Implementation
//
//
#if defined(MTB_ASSERT_IMPLEMENTATION)

#if !defined(MTB_IMPL_mtb_assert)
#define MTB_IMPL_mtb_assert


static bool DefaultAssertHandler(
  char const* FileName,
  size_t      Line,
  char const* FunctionName,
  char const* Expression,
  char const* Message)
{
  return true;
}

static mtb_AssertHandler mtb_GlobalAssertHandler = &DefaultAssertHandler;

mtb_AssertHandler mtb::
GetAssertHandler()
{
  return mtb_GlobalAssertHandler;
}

void mtb::
SetAssertHandler(mtb_AssertHandler AssertHandler)
{
  mtb_GlobalAssertHandler = AssertHandler;
}

bool mtb::
OnFailedCheck(
  char const* FileName,
  size_t      Line,
  char const* FunctionName,
  char const* Expression,
  char const* MessageFormat,
  ...)
{
  if(!mtb_GlobalAssertHandler)
  {
    // If there's no assert handler, always trigger a debug break.
    return true;
  }

  // TODO: Format message.
  // NOTE: Ensure message is a valid string, so at least let it be empty.
  char const* Message = MessageFormat ? MessageFormat : "";

  return (*mtb_GlobalAssertHandler)(FileName, Line, FunctionName, Expression, Message);
}


#endif // !defined(MTB_IMPL_mtb_assert)
#endif // defined(MTB_ASSERT_IMPLEMENTATION)

