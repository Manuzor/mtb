#if !defined(MTB_HEADER_mtb_conv)
#define MTB_HEADER_mtb_conv

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_CONV_IMPLEMENTATION)
  #define MTB_CONV_IMPLEMENTATION
#endif

#if defined(MTB_CONV_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.hpp"
#include "mtb_slice.hpp"

namespace mtb
{

template<typename T>
struct convert_string_to_number_result
{
  // Whether conversion was successful or not.
  bool Success;

  // The actual value.
  T Value;

  // What is left after parsing the value.
  slice<char const> RemainingSource;
};


//
// Conversion: String -> Floating Point
//

convert_string_to_number_result<double>
ImplConvertStringToDouble(slice<char const> Source, double Fallback);

template<typename FloatType>
struct impl_convert_string_to_floating_point_helper
{
  template<typename CharType>
  static inline convert_string_to_number_result<FloatType>
  Do(slice<CharType> Source, FloatType Fallback = NaN<FloatType>())
  {
    auto StringToDoubleResult = ImplConvertStringToDouble(Coerce<slice<char const>>(Source), Cast<double>(Fallback));
    convert_string_to_number_result<FloatType> Result;
    Result.Success = StringToDoubleResult.Success;
    Result.Value = (FloatType)StringToDoubleResult.Value;
    Result.RemainingSource = StringToDoubleResult.RemainingSource;
    return Result;
  }
};

template<> struct impl_convert<double, slice<char>>        : impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<double, slice<char const>>  : impl_convert_string_to_floating_point_helper<double> {};
template<> struct impl_convert<float,  slice<char>>        : impl_convert_string_to_floating_point_helper<float>  {};
template<> struct impl_convert<float,  slice<char const>>  : impl_convert_string_to_floating_point_helper<float>  {};

//
// Conversion: String -> Integer
//

convert_string_to_number_result<u64>
ImplConvertStringToInteger(slice<char const> Source, u64 Fallback);

convert_string_to_number_result<i64>
ImplConvertStringToInteger(slice<char const> Source, i64 Fallback);


template<typename MaxIntegerType, typename IntegerType>
struct impl_convert_string_to_integer_helper
{
  template<typename CharType>
  static inline convert_string_to_number_result<IntegerType>
  Do(slice<CharType> String, IntegerType Fallback = 0)
  {
    auto StringToIntResult = ImplConvertStringToInteger(Coerce<slice<char const>>(String), Cast<MaxIntegerType>(Fallback));
    convert_string_to_number_result<IntegerType> Result;
    Result.Success = StringToIntResult.Success;
    Result.Value = Convert<IntegerType>(StringToIntResult.Value);
    Result.RemainingSource = StringToIntResult.RemainingSource;
    return Result;
  }
};

template<> struct impl_convert<i8,  slice<char>>       : impl_convert_string_to_integer_helper<i64, i8>  {};
template<> struct impl_convert<i8,  slice<char const>> : impl_convert_string_to_integer_helper<i64, i8>  {};
template<> struct impl_convert<i16, slice<char>>       : impl_convert_string_to_integer_helper<i64, i16> {};
template<> struct impl_convert<i16, slice<char const>> : impl_convert_string_to_integer_helper<i64, i16> {};
template<> struct impl_convert<i32, slice<char>>       : impl_convert_string_to_integer_helper<i64, i32> {};
template<> struct impl_convert<i32, slice<char const>> : impl_convert_string_to_integer_helper<i64, i32> {};
template<> struct impl_convert<i64, slice<char>>       : impl_convert_string_to_integer_helper<i64, i64> {};
template<> struct impl_convert<i64, slice<char const>> : impl_convert_string_to_integer_helper<i64, i64> {};

template<> struct impl_convert<u8,  slice<char>>       : impl_convert_string_to_integer_helper<u64, u8>  {};
template<> struct impl_convert<u8,  slice<char const>> : impl_convert_string_to_integer_helper<u64, u8>  {};
template<> struct impl_convert<u16, slice<char>>       : impl_convert_string_to_integer_helper<u64, u16> {};
template<> struct impl_convert<u16, slice<char const>> : impl_convert_string_to_integer_helper<u64, u16> {};
template<> struct impl_convert<u32, slice<char>>       : impl_convert_string_to_integer_helper<u64, u32> {};
template<> struct impl_convert<u32, slice<char const>> : impl_convert_string_to_integer_helper<u64, u32> {};
template<> struct impl_convert<u64, slice<char>>       : impl_convert_string_to_integer_helper<u64, u64> {};
template<> struct impl_convert<u64, slice<char const>> : impl_convert_string_to_integer_helper<u64, u64> {};


//
// ==============================================
//

struct convert_number_to_string_result
{
  bool Success;
  slice<char> Value;
};


//
// Conversion: String -> Floating Point
//

// TODO


//
// Conversion: Integer -> String
//

convert_number_to_string_result
ImplConvertIntegerToString(i64 Integer, slice<char> Buffer);

convert_number_to_string_result
ImplConvertIntegerToString(u64 Integer, slice<char> Buffer);

template<typename MaxIntegerType, typename IntegerType>
struct impl_convert_integer_to_string_helper
{
  static inline convert_number_to_string_result
  Do(IntegerType Integer, slice<char> Buffer)
  {
    return ImplConvertIntegerToString((MaxIntegerType)Integer, Buffer);
  }
};

template<> struct impl_convert<slice<char>,        i8>  : impl_convert_integer_to_string_helper<i64, i8>   {};
template<> struct impl_convert<slice<char const>,  i8>  : impl_convert_integer_to_string_helper<i64, i8>   {};
template<> struct impl_convert<slice<char>,        i16> : impl_convert_integer_to_string_helper<i64, i16>  {};
template<> struct impl_convert<slice<char const>,  i16> : impl_convert_integer_to_string_helper<i64, i16>  {};
template<> struct impl_convert<slice<char>,        i32> : impl_convert_integer_to_string_helper<i64, i32>  {};
template<> struct impl_convert<slice<char const>,  i32> : impl_convert_integer_to_string_helper<i64, i32>  {};
template<> struct impl_convert<slice<char>,        i64> : impl_convert_integer_to_string_helper<i64, i64>  {};
template<> struct impl_convert<slice<char const>,  i64> : impl_convert_integer_to_string_helper<i64, i64>  {};

template<> struct impl_convert<slice<char>,        u8>  : impl_convert_integer_to_string_helper<u64, u8>  {};
template<> struct impl_convert<slice<char const>,  u8>  : impl_convert_integer_to_string_helper<u64, u8>  {};
template<> struct impl_convert<slice<char>,        u16> : impl_convert_integer_to_string_helper<u64, u16> {};
template<> struct impl_convert<slice<char const>,  u16> : impl_convert_integer_to_string_helper<u64, u16> {};
template<> struct impl_convert<slice<char>,        u32> : impl_convert_integer_to_string_helper<u64, u32> {};
template<> struct impl_convert<slice<char const>,  u32> : impl_convert_integer_to_string_helper<u64, u32> {};
template<> struct impl_convert<slice<char>,        u64> : impl_convert_integer_to_string_helper<u64, u64> {};
template<> struct impl_convert<slice<char const>,  u64> : impl_convert_integer_to_string_helper<u64, u64> {};

} // namespace mtb
#endif // !defined(MTB_HEADER_mtb_conv)


// ==============
// Implementation
// ==============

#if defined(MTB_CONV_IMPLEMENTATION)
#if !defined(MTB_IMPL_mtb_conv)
#define MTB_IMPL_mtb_conv


static mtb::slice<char const>
mtb_TrimWhitespaceFront(mtb::slice<char const> String)
{
  using namespace mtb;

  while(true)
  {
    if(LengthOf(String) == 0)
      return String;

    if(!IsWhitespace(String[0]))
      return String;

    String = TrimFront(String, 1);
  }
}

/// If a '+' was consumed, `1` is returned and \a OriginalSource is advanced by 1,
/// else, if a '-' was consumed, `-1` is returned and \a OriginalSource is advanced by 1,
/// otherwise 0 is returned and Source will not be modified.
static int
mtb_ConsumeSign(mtb::slice<char const>* Source)
{
  int Result{};

  if(*Source)
  {
    if((*Source)[0] == '+')
    {
      *Source = TrimFront(*Source, 1);
      Result = 1;
    }
    else if((*Source)[0] == '-')
    {
      *Source = TrimFront(*Source, 1);
      Result = -1;
    }
  }

  return Result;
}

auto mtb::
ImplConvertStringToDouble(slice<char const> Source, double Fallback)
  -> convert_string_to_number_result<double>
{
  convert_string_to_number_result<double> Result;
  Result.Success = false;
  Result.Value = Fallback;
  Result.RemainingSource = Source;

  Source = mtb_TrimWhitespaceFront(Source);
  if(Source)
  {
    bool HasSign = mtb_ConsumeSign(&Source) < 0;

    if(Source)
    {
      // Parse all parts of a floating point numbers
      convert_string_to_number_result<u64> NumericResult;
      convert_string_to_number_result<u64> ExponentResult;

      // Numeric part
      NumericResult = Convert<u64>(Source, 0.0f);
      Source = NumericResult.RemainingSource;
      double Value = NumericResult.Value;

      // Decimal part
      bool HasDecimalPart{};
      u64 DecimalValue{};
      u64 DecimalDivider = 1;
      if(Source && Source[0] == '.')
      {
        Source = TrimFront(Source, 1);
        while(Source && IsDigit(Source[0]))
        {
          u64 NewDigit = (u64)(Source[0] - '0');
          DecimalValue = (10 * DecimalValue) + NewDigit;
          HasDecimalPart = true;
          DecimalDivider *= 10;
          Source = TrimFront(Source, 1);
        }

        Value += (double)DecimalValue / (double)DecimalDivider;
      }

      if(NumericResult.Success || HasDecimalPart)
      {
        // Parse exponent, if any.
        if(Source && (Source[0] == 'e' || Source[0] == 'E'))
        {
          auto ExponentSource = TrimFront(Source, 1);
          bool ExponentHasSign = mtb_ConsumeSign(&ExponentSource) < 0;
          ExponentResult = Convert<u64>(ExponentSource);
          if(ExponentResult.Success)
          {
            Source = ExponentResult.RemainingSource;

            u64 ExponentPart = ExponentResult.Value;
            i64 ExponentValue = 1;
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
        Result.RemainingSource = Source;
        Result.Value = HasSign ? -Value : Value;
      }
    }
  }

  return Result;
}

template<typename IntegerType>
mtb::convert_string_to_number_result<IntegerType>
ImplConvertStringToIntegerHelper(mtb::slice<char const> Source, IntegerType Fallback)
{
  using namespace mtb;

  convert_string_to_number_result<IntegerType> Result;
  Result.Success = false;
  Result.Value = Fallback;
  Result.RemainingSource = Source;

  if(Source)
  {
    Source = mtb_TrimWhitespaceFront(Source);

    bool HasSign = false;
    switch(Source[0])
    {
    case '+':
      Source = TrimFront(Source, 1);
      break;
    case '-':
      HasSign = true;
      Source = TrimFront(Source, 1);
      break;
    default:
      break;
    }

    u64 NumericalPart = 0;
    bool HasNumericalPart = false;

    while(LengthOf(Source) > 0 && IsDigit(Source[0]))
    {
      NumericalPart *= 10;
      NumericalPart += Source[0] - '0';
      HasNumericalPart = true;
      Source = TrimFront(Source, 1);
    }

    if(HasNumericalPart)
    {
      auto Value = Convert<IntegerType>(NumericalPart);

      if(HasSign)
      {
        if(IntIsSigned<IntegerType>())
        {
          Result.Value = Negate(Value);
          Result.Success = true;
        }
        else
        {
          // Unsigned types cannot have a '-' sign.
        }
      }
      else
      {
        Result.Value = Value;
        Result.Success = true;
      }
    }
  }

  if(Result.Success)
    Result.RemainingSource = Source;

  return Result;
}

auto mtb::
ImplConvertStringToInteger(slice<char const> Source, u64 Fallback)
  -> convert_string_to_number_result<u64>
{
  return ImplConvertStringToIntegerHelper<u64>(Source, Fallback);
}

auto mtb::
ImplConvertStringToInteger(slice<char const> Source, i64 Fallback)
  -> convert_string_to_number_result<i64>
{
  return ImplConvertStringToIntegerHelper<i64>(Source, Fallback);
}

template<typename IntegerType>
mtb::convert_number_to_string_result
ImplConvertIntegerToStringHelper(IntegerType Integer, mtb::slice<char> Buffer)
{
  using namespace mtb;

  convert_number_to_string_result Result{};

  if(Buffer)
  {
    if(Integer == 0)
    {
      Buffer[0] = '0';
      Result.Success = true;
      Result.Value = Slice(Buffer, 0, 1);
    }
    else
    {
      size_t NumChars = 0;
      if(Integer < 0)
      {
        Buffer[NumChars++] = '-';
        Integer = Negate(Integer);
      }

      while(Integer > 0)
      {
        auto const Digit = (char)(Integer % 10);
        Buffer[NumChars++] = '0' + Digit;
        Integer /= 10;
      }

      Result.Value = Slice(Buffer, 0, NumChars);

      // Result.Value now contains the digits in reverse order, so we swap them around.
      ReverseElements(Result.Value);

      Result.Success = true;
    }
  }

  return Result;
}

auto mtb::
ImplConvertIntegerToString(i64 Integer, slice<char> Buffer)
  -> mtb::convert_number_to_string_result
{
  return ImplConvertIntegerToStringHelper<i64>(Integer, Buffer);
}

auto mtb::
ImplConvertIntegerToString(u64 Integer, slice<char> Buffer)
  -> mtb::convert_number_to_string_result
{
  return ImplConvertIntegerToStringHelper<u64>(Integer, Buffer);
}

#endif // !defined(MTB_IMPL_mtb_conv)
#endif // defined(MTB_CONV_IMPLEMENTATION)
