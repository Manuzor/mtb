#if !defined(MTB_HEADER_mtb_conv)
#define MTB_HEADER_mtb_conv

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_CONV_IMPLEMENTATION)
  #define MTB_CONV_IMPLEMENTATION
#endif

#if defined(MTB_CONV_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.h"


#define MTB_DEFINE_PARSE_STRING_RESULT(NAME, VALUE_TYPE) \
struct NAME \
{ \
  bool Success; \
  size_t RemainingSourceLen; \
  char const* RemainingSourcePtr; \
  VALUE_TYPE Value; \
}


//
// Convert: String -> Floating point
//

MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_f64, mtb_f64);
mtb_parse_string_result_f64
mtb_ParseString_f64(size_t SourceLen, char const* SourcePtr, mtb_f64 Fallback = MTB_NaN_f64);


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_f32, mtb_f32);
MTB_INLINE mtb_parse_string_result_f32
mtb_ParseString_f32(size_t SourceLen, char const* SourcePtr, mtb_f32 Fallback = MTB_NaN_f64)
{
  mtb_parse_string_result_f64 Result_f64 = mtb_ParseString_f64(SourceLen, SourcePtr, (mtb_f64)Fallback);
  mtb_parse_string_result_f32 Result_f32{ Result_f64.Success, Result_f64.RemainingSourceLen, Result_f64.RemainingSourcePtr };
  Result_f32.Value = (mtb_f32)Result_f64.Value;
  return Result_f32;
}


//
// Convert: String -> Unsigned integers
//

MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_u64, mtb_u64);
mtb_parse_string_result_u64
mtb_ParseString_u64(size_t SourceLen, char const* SourcePtr, mtb_u64 Fallback);


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_u32, mtb_u32);
MTB_INLINE mtb_parse_string_result_u32
mtb_ParseString_u32(size_t SourceLen, char const* SourcePtr, mtb_u32 Fallback)
{
  mtb_parse_string_result_u64 Result_u64 = mtb_ParseString_u64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_u32 Result_u32{ Result_u64.Success, Result_u64.RemainingSourceLen, Result_u64.RemainingSourcePtr };
  Result_u32.Value = mtb_SafeConvert_u32(Result_u64.Value);
}


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_u16, mtb_u16);
MTB_INLINE mtb_parse_string_result_u16
mtb_ParseString_u16(size_t SourceLen, char const* SourcePtr, mtb_u16 Fallback)
{
  mtb_parse_string_result_u64 Result_u64 = mtb_ParseString_u64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_u16 Result_u16{ Result_u64.Success, Result_u64.RemainingSourceLen, Result_u64.RemainingSourcePtr };
  Result_u16.Value = mtb_SafeConvert_u16(Result_u64.Value);
}


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_u08, mtb_u08);
MTB_INLINE mtb_parse_string_result_u08
mtb_ParseString_u08(size_t SourceLen, char const* SourcePtr, mtb_u08 Fallback)
{
  mtb_parse_string_result_u64 Result_u64 = mtb_ParseString_u64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_u08 Result_u08{ Result_u64.Success, Result_u64.RemainingSourceLen, Result_u64.RemainingSourcePtr };
  Result_u08.Value = mtb_SafeConvert_u08(Result_u64.Value);
}


//
// Convert: String -> Signed integers
//

MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_s64, mtb_s64);
mtb_parse_string_result_s64
mtb_ParseString_s64(size_t SourceLen, char const* SourcePtr, mtb_s64 Fallback);


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_s32, mtb_s32);
MTB_INLINE mtb_parse_string_result_s32
mtb_ParseString_s32(size_t SourceLen, char const* SourcePtr, mtb_s32 Fallback)
{
  mtb_parse_string_result_s64 Result_s64 = mtb_ParseString_s64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_s32 Result_s32{ Result_s64.Success, Result_s64.RemainingSourceLen, Result_s64.RemainingSourcePtr };
  Result_s32.Value = mtb_SafeConvert_s32(Result_s64.Value);
}


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_s16, mtb_s16);
MTB_INLINE mtb_parse_string_result_s16
mtb_ParseString_s16(size_t SourceLen, char const* SourcePtr, mtb_s16 Fallback)
{
  mtb_parse_string_result_s64 Result_s64 = mtb_ParseString_s64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_s16 Result_s16{ Result_s64.Success, Result_s64.RemainingSourceLen, Result_s64.RemainingSourcePtr };
  Result_s16.Value = mtb_SafeConvert_s16(Result_s64.Value);
}


MTB_DEFINE_PARSE_STRING_RESULT(mtb_parse_string_result_s08, mtb_s08);
MTB_INLINE mtb_parse_string_result_s08
mtb_ParseString_s08(size_t SourceLen, char const* SourcePtr, mtb_s08 Fallback)
{
  mtb_parse_string_result_s64 Result_s64 = mtb_ParseString_s64(SourceLen, SourcePtr, Fallback);
  mtb_parse_string_result_s08 Result_s08{ Result_s64.Success, Result_s64.RemainingSourceLen, Result_s64.RemainingSourcePtr };
  Result_s08.Value = mtb_SafeConvert_s08(Result_s64.Value);
}


//
// ==============================================
//

struct mtb_to_string_result
{
  bool Success;
  size_t StrLen;
  char* StrPtr;
};


//
// Conversion: String -> Floating Point
//

// TODO


//
// Conversion: Integer -> String
//

mtb_to_string_result
mtb_ToString(mtb_s64 Value, size_t BufferSize, char* BufferPtr);

MTB_INLINE mtb_to_string_result mtb_ToString(mtb_s32 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_s64)Value, BufferSize, BufferPtr); }
MTB_INLINE mtb_to_string_result mtb_ToString(mtb_s16 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_s64)Value, BufferSize, BufferPtr); }
MTB_INLINE mtb_to_string_result mtb_ToString(mtb_s08 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_s64)Value, BufferSize, BufferPtr); }


mtb_to_string_result
mtb_ToString(mtb_u64 Value, size_t BufferSize, char* BufferPtr);

MTB_INLINE mtb_to_string_result mtb_ToString(mtb_u32 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_u64)Value, BufferSize, BufferPtr); }
MTB_INLINE mtb_to_string_result mtb_ToString(mtb_u16 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_u64)Value, BufferSize, BufferPtr); }
MTB_INLINE mtb_to_string_result mtb_ToString(mtb_u08 Value, size_t BufferSize, char* BufferPtr) { return mtb_ToString((mtb_u64)Value, BufferSize, BufferPtr); }


#endif // !defined(MTB_HEADER_mtb_conv)


// ==============
// Implementation
// ==============

#if defined(MTB_CONV_IMPLEMENTATION)
#if !defined(MTB_IMPL_mtb_conv)
#define MTB_IMPL_mtb_conv

#include "mtb_memory.h"

static size_t
mtb_TrimWhitespaceFront(size_t* SourceLen, char const** SourcePtr)
{
  size_t Len = *SourceLen;
  char const* Ptr = *SourcePtr;

  while(Len && mtb_IsWhitespace(Ptr[0]))
  {
    ++Ptr;
    --Len;
  }

  size_t NumTrimmed = *SourceLen - Len;
  *SourceLen = Len;
  *SourcePtr = Ptr;
  return NumTrimmed;
}

/// If a '+' was consumed, `1` is returned and \a SourcePtr is advanced by 1,
/// else, if a '-' was consumed, `-1` is returned and \a SourcePtr is advanced by 1,
/// otherwise 0 is returned and \a SourcePtr will not be modified.
static int
mtb_ConsumeSign(size_t* SourceLen, char const** SourcePtr)
{
  int Result{};
  size_t Len = *SourceLen;
  char const* Ptr = *SourcePtr;

  if(Len)
  {
    if(Ptr[0] == '+')
    {
      ++Ptr;
      --Len;
      Result = 1;
    }
    else if(Ptr[0] == '-')
    {
      ++Ptr;
      --Len;
      Result = -1;
    }
  }

  *SourceLen = Len;
  *SourcePtr = Ptr;
  return Result;
}

mtb_parse_string_result_f64
mtb_ParseString_f64(size_t SourceLen, char const* SourcePtr, mtb_f64 Fallback)
{
  mtb_parse_string_result_f64 Result{};
  Result.Value = Fallback;
  Result.RemainingSourceLen = SourceLen;
  Result.RemainingSourcePtr = SourcePtr;

  size_t Len = SourceLen;
  char const* Ptr = SourcePtr;

  mtb_TrimWhitespaceFront(&Len, &Ptr);
  if(Len)
  {
    bool HasSign = mtb_ConsumeSign(&Len, &Ptr) < 0;

    // Parse all parts of a floating point number.
    if(Len)
    {
      // Numeric part
      mtb_parse_string_result_u64 NumericResult = mtb_ParseString_u64(Len, Ptr, (mtb_u64)-1);
      Len = NumericResult.RemainingSourceLen;
      Ptr = NumericResult.RemainingSourcePtr;
      double Value = (double)NumericResult.Value;

      // Decimal part
      bool HasDecimalPart = false;
      mtb_u64 DecimalValue = 0;
      mtb_u64 DecimalDivider = 1;
      if(Len && Ptr[0] == '.')
      {
        ++Ptr;
        --Len;
        while(Len && mtb_IsDigit(Ptr[0]))
        {
          mtb_u64 NewDigit = (mtb_u64)(Ptr[0] - '0');
          DecimalValue = (10 * DecimalValue) + NewDigit;
          HasDecimalPart = true;
          DecimalDivider *= 10;
          ++Ptr;
          --Len;
        }

        Value += (double)DecimalValue / (double)DecimalDivider;
      }

      if(NumericResult.Success || HasDecimalPart)
      {
        // Parse exponent, if any.
        if(Len && (Ptr[0] == 'e' || Ptr[0] == 'E'))
        {
          size_t ExponentSourceLen = Len - 1;
          char const* ExponentSourcePtr = Ptr + 1;
          bool ExponentHasSign = mtb_ConsumeSign(&ExponentSourceLen, &ExponentSourcePtr) < 0;

          mtb_parse_string_result_u64 ExponentResult = mtb_ParseString_u64(ExponentSourceLen, ExponentSourcePtr, (mtb_u64)-1);
          if(ExponentResult.Success)
          {
            Len = ExponentResult.RemainingSourceLen;
            Ptr = ExponentResult.RemainingSourcePtr;

            mtb_u64 ExponentPart = ExponentResult.Value;
            mtb_s64 ExponentValue = 1;
            while(ExponentPart > 0)
            {
              ExponentValue *= 10;
              --ExponentPart;
            }

            if(ExponentHasSign) Value /= ExponentValue;
            else                Value *= ExponentValue;
          }
        }

        Result.Success = true;
        Result.RemainingSourceLen = Len;
        Result.RemainingSourcePtr = Ptr;
        Result.Value = HasSign ? -Value : Value;
      }
    }
  }

  return Result;
}

mtb_parse_string_result_u64
mtb_ParseString_u64(size_t SourceLen, char const* SourcePtr, mtb_u64 Fallback)
{
  mtb_parse_string_result_u64 Result{};
  Result.Value = Fallback;
  Result.RemainingSourceLen = SourceLen;
  Result.RemainingSourcePtr = SourcePtr;

  size_t Len = SourceLen;
  char const* Ptr = SourcePtr;

  if(Len)
  {
    mtb_TrimWhitespaceFront(&Len, &Ptr);
    bool HasSign = mtb_ConsumeSign(&Len, &Ptr) < 0;
    if(!HasSign)
    {
      mtb_u64 NumericalPart = 0;
      bool HasNumericalPart = false;

      while(Len && mtb_IsDigit(Ptr[0]))
      {
        NumericalPart = (10 * NumericalPart) + (*Ptr - '0');
        HasNumericalPart = true;
        --Len;
        ++Ptr;
      }

      if(HasNumericalPart)
      {
        Result.Value = NumericalPart;
        Result.Success = true;
      }
    }
  }

  if(Result.Success)
  {
    Result.RemainingSourceLen = Len;
    Result.RemainingSourcePtr = Ptr;
  }

  return Result;
}

mtb_parse_string_result_s64
mtb_ParseString_s64(size_t SourceLen, char const* SourcePtr, mtb_s64 Fallback)
{
  mtb_parse_string_result_s64 Result{};
  Result.Value = Fallback;
  Result.RemainingSourceLen = SourceLen;
  Result.RemainingSourcePtr = SourcePtr;

  size_t Len = SourceLen;
  char const* Ptr = SourcePtr;

  if(Len)
  {
    mtb_TrimWhitespaceFront(&Len, &Ptr);
    bool HasSign = mtb_ConsumeSign(&Len, &Ptr) < 0;

    mtb_u64 NumericalPart = 0;
    bool HasNumericalPart = false;

    while(Len && mtb_IsDigit(Ptr[0]))
    {
      NumericalPart = (10 * NumericalPart) + (*Ptr - '0');
      HasNumericalPart = true;
      --Len;
      ++Ptr;
    }

    if(HasNumericalPart)
    {
      if(HasSign)
      {
        if(NumericalPart <= (mtb_u64)MTB_MaxValue_s64 + 1)
        {
          if(NumericalPart == (mtb_u64)MTB_MaxValue_s64 + 1)
          {
            Result.Value = MTB_MinValue_s64;
          }
          else
          {
            Result.Value = -(mtb_s64)NumericalPart;
          }

          Result.Success = true;
        }
      }
      else
      {
        if(NumericalPart <= MTB_MaxValue_s64)
        {
          Result.Value = NumericalPart;
          Result.Success = true;
        }
      }
    }
  }

  if(Result.Success)
  {
    Result.RemainingSourceLen = Len;
    Result.RemainingSourcePtr = Ptr;
  }

  return Result;
}


mtb_to_string_result
mtb_ToString(mtb_s64 Value, size_t BufferSize, char* BufferPtr)
{
  #if MTB_FLAG(INTERNAL)
    mtb_s64 OriginalValue = Value;
  #endif
  mtb_to_string_result Result{};
  Result.StrPtr = BufferPtr;

  if(BufferSize)
  {
    size_t NumChars = 0;
    if(Value < 0)
    {
      BufferPtr[NumChars++] = '-';
        // TODO: What if Value == MTB_MinValue_s64?
      Value = -Value;
    }

    while(Value > 0)
    {
      char Digit = (char)(Value % 10);
      BufferPtr[NumChars++] = '0' + Digit;
      Value /= 10;
    }

    Result.StrLen = NumChars;

      // Characters are now in reverse order, so we swap them around.
    mtb_ReverseBytesInPlace(Result.StrLen, Result.StrPtr);

    Result.Success = true;
  }

  return Result;
}

mtb_to_string_result
mtb_ToString(mtb_u64 Value, size_t BufferSize, char* BufferPtr)
{
  #if MTB_FLAG(INTERNAL)
    mtb_u64 OriginalValue = Value;
  #endif
  mtb_to_string_result Result{};
  Result.StrPtr = BufferPtr;

  if(BufferSize)
  {
    size_t NumChars = 0;
    while(Value > 0)
    {
      char Digit = (char)(Value % 10);
      BufferPtr[NumChars++] = '0' + Digit;
      Value /= 10;
    }

    Result.StrLen = NumChars;

      // Characters are now in reverse order, so we swap them around.
    mtb_ReverseBytesInPlace(Result.StrLen, Result.StrPtr);

    Result.Success = true;
  }

  return Result;
}


#endif // !defined(MTB_IMPL_mtb_conv)
#endif // defined(MTB_CONV_IMPLEMENTATION)
