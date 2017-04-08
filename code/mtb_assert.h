#if !defined(MTB_HEADER_mtb_assert)
#define MTB_HEADER_mtb_assert

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_ASSERT_IMPLEMENTATION)
  #define MTB_ASSERT_IMPLEMENTATION
#endif

#if defined(MTB_ASSERT_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_platform.h"


/// Returning \c true means that the a debug break is required.
using mtb_assert_handler = bool(*)(
  char const* FileName,     //< The name of the file where the assert is.
  size_t      Line,         //< The line number in the file where the assert is.
  char const* FunctionName, //< The name of the function where the assert is.
  char const* Expression,   //< The condition as captured by the MTB_Assert macro. May be nullptr in case of MTB_Fail.
  char const* Message);     //< The user defined message.

mtb_assert_handler mtb_GetAssertHandler();
void mtb_SetAssertHandler(mtb_assert_handler AssertHandler);

bool mtb_OnFailedCheck(
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
        if(mtb_OnFailedCheck(MTB_CURRENT_FILE, MTB_CURRENT_LINE, MTB_CURRENT_FUNCTION, #CONDITION, __VA_ARGS__)) \
        { \
          MTB_DebugBreak(); \
        } \
      } \
    } while(0)
#endif


//
// Define assertion macros
//

#if !defined(MTB_AssertDebug)
  #if MTB_FLAG(DEBUG)
    #define MTB_AssertDebug MTB_Require
  #else
    #define MTB_AssertDebug(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_AssertRelease)
  #if MTB_FLAG(RELEASE)
    #define MTB_AssertRelease MTB_Require
  #else
    #define MTB_AssertRelease(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_AssertInternal)
  #if MTB_FLAG(INTERNAL)
    #define MTB_AssertInternal MTB_Require
  #else
    #define MTB_AssertInternal(...) MTB_NOP
  #endif
#endif

#if !defined(MTB_BoundsCheck)
  #if MTB_FLAG(BOUNDS_CHECKING)
    #define MTB_BoundsCheck MTB_Require
  #else
    #define MTB_BoundsCheck(...) MTB_NOP
  #endif
#endif


//
// Macro to mark an invalid code path.
//
#if !defined(MTB_Fail)
  #define MTB_Fail(...) \
    do \
    { \
      if(mtb_OnFailedCheck(MTB_CURRENT_FILE, MTB_CURRENT_LINE, MTB_CURRENT_FUNCTION, nullptr, __VA_ARGS__)) \
      { \
        MTB_DebugBreak(); \
      } \
    } while(0)
#endif

#if !defined(MTB_NOT_IMPLEMENTED)
  #define MTB_NOT_IMPLEMENTED MTB_Fail("Not implemented.")
#endif

#if !defined(MTB_INVALID_CODE_PATH)
  #define MTB_INVALID_CODE_PATH MTB_Fail("Invalid code path.")
#endif


#endif // !defined(MTB_HEADER_mtb_assert)


//
// Implementation
//
#if defined(MTB_ASSERT_IMPLEMENTATION)

#if !defined(MTB_IMPL_mtb_assert)
#define MTB_IMPL_mtb_assert


static bool mtb__DefaultAssertHandler(
  char const* FileName,
  size_t      Line,
  char const* FunctionName,
  char const* Expression,
  char const* Message)
{
  // Don't do anything special, just trigger a debug break by returning true.
  return true;
}

static mtb_assert_handler mtb__GlobalAssertHandler = &mtb__DefaultAssertHandler;

mtb_assert_handler
mtb_GetAssertHandler()
{
  return mtb__GlobalAssertHandler;
}

void
mtb_SetAssertHandler(mtb_assert_handler AssertHandler)
{
  mtb__GlobalAssertHandler = AssertHandler;
}

bool
mtb_OnFailedCheck(
  char const* FileName,
  size_t      Line,
  char const* FunctionName,
  char const* Expression,
  char const* MessageFormat,
  ...)
{
  bool Result = true;

  if(mtb__GlobalAssertHandler)
  {
  // TODO: Format message.
  // NOTE: Ensure message is a valid string, so at least let it be empty.
    char const* Message = MessageFormat ? MessageFormat : "";
    Result = (*mtb__GlobalAssertHandler)(FileName, Line, FunctionName, Expression, Message);
  }

  return Result;
}


#endif // !defined(MTB_IMPL_mtb_assert)
#endif // defined(MTB_ASSERT_IMPLEMENTATION)

