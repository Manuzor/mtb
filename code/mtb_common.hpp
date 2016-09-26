#if !defined(MTB_HEADER_mtb_common)
#define MTB_HEADER_mtb_common

#include "mtb_platform.hpp"

#if defined(MTB_IMPLEMENTATION)
  #define MTB_COMMON_IMPLEMENTATION
#endif

namespace mtb
{
//
// Primitive array types.
//

/// Defines some array variants of types for better readability when used as
/// function parameters.
///
/// For example, a function `Foo` that accepts and array of 4 floats by
/// reference-to-const looks like this:
/// \code
/// void Foo(float const (&ParamName)[4]);
/// \endcode
///
/// Using these typedefs, this can be transformed into:
/// \code
/// void Foo(float_4 const& ParamName);
/// \endcode
#define MTB_DefineArrayTypes(TheType)\
  using TheType##_1   = TheType[1];\
  using TheType##_2   = TheType[2];\
  using TheType##_3   = TheType[3];\
  using TheType##_4   = TheType[4];\
  using TheType##_1x1 = TheType##_1[1];\
  using TheType##_1x2 = TheType##_1[2];\
  using TheType##_1x3 = TheType##_1[3];\
  using TheType##_1x4 = TheType##_1[4];\
  using TheType##_2x1 = TheType##_2[1];\
  using TheType##_2x2 = TheType##_2[2];\
  using TheType##_2x3 = TheType##_2[3];\
  using TheType##_2x4 = TheType##_2[4];\
  using TheType##_3x1 = TheType##_3[1];\
  using TheType##_3x2 = TheType##_3[2];\
  using TheType##_3x3 = TheType##_3[3];\
  using TheType##_3x4 = TheType##_3[4];\
  using TheType##_4x1 = TheType##_4[1];\
  using TheType##_4x2 = TheType##_4[2];\
  using TheType##_4x3 = TheType##_4[3];\
  using TheType##_4x4 = TheType##_4[4]

MTB_DefineArrayTypes(i8);
MTB_DefineArrayTypes(i16);
MTB_DefineArrayTypes(i32);
MTB_DefineArrayTypes(i64);

MTB_DefineArrayTypes(u8);
MTB_DefineArrayTypes(u16);
MTB_DefineArrayTypes(u32);
MTB_DefineArrayTypes(u64);

MTB_DefineArrayTypes(f32);
MTB_DefineArrayTypes(f64);


//
// ============================
//

struct memory_size
{
  // In bytes.
  u64 Value;

  constexpr
  operator size_t() const
  {
    // TODO: Ensure safe conversion?
    return (size_t)Value;
  }
};

constexpr bool operator ==(memory_size A, memory_size B) { return A.Value == B.Value; }
constexpr bool operator !=(memory_size A, memory_size B) { return A.Value != B.Value; }
constexpr bool operator < (memory_size A, memory_size B) { return A.Value <  B.Value; }
constexpr bool operator <=(memory_size A, memory_size B) { return A.Value <= B.Value; }
constexpr bool operator > (memory_size A, memory_size B) { return A.Value >  B.Value; }
constexpr bool operator >=(memory_size A, memory_size B) { return A.Value >= B.Value; }

constexpr memory_size operator +(memory_size A, memory_size B) { return { A.Value + B.Value }; }
constexpr memory_size operator -(memory_size A, memory_size B) { return { A.Value - B.Value }; }
constexpr memory_size operator *(memory_size A, u64 Scale) { return { A.Value * Scale };   }
constexpr memory_size operator *(u64 Scale, memory_size A) { return { Scale * A.Value };   }
constexpr memory_size operator /(memory_size A, u64 Scale) { return { A.Value / Scale };   }

void operator +=(memory_size& A, memory_size B);
void operator -=(memory_size& A, memory_size B);
void operator *=(memory_size& A, u64 Scale);
void operator /=(memory_size& A, u64 Scale);

constexpr memory_size Bytes(u64 Amount) { return { Amount }; }
constexpr u64 ToBytes(memory_size Size) { return Size.Value; }

constexpr memory_size KiB(u64 Amount) { return { Amount * 1024 }; }
constexpr memory_size MiB(u64 Amount) { return { Amount * 1024 * 1024 }; }
constexpr memory_size GiB(u64 Amount) { return { Amount * 1024 * 1024 * 1024 }; }
constexpr memory_size TiB(u64 Amount) { return { Amount * 1024 * 1024 * 1024 * 1024 }; }

constexpr memory_size KB(u64 Amount) { return { Amount * 1000 }; }
constexpr memory_size MB(u64 Amount) { return { Amount * 1000 * 1000 }; }
constexpr memory_size GB(u64 Amount) { return { Amount * 1000 * 1000 * 1000 }; }
constexpr memory_size TB(u64 Amount) { return { Amount * 1000 * 1000 * 1000 * 1000 }; }

template<typename OutputType = float> constexpr OutputType ToKiB(memory_size Size) { return OutputType(double(Size.Value) / 1024); }
template<typename OutputType = float> constexpr OutputType ToMiB(memory_size Size) { return OutputType(double(Size.Value) / 1024 / 1024); }
template<typename OutputType = float> constexpr OutputType ToGiB(memory_size Size) { return OutputType(double(Size.Value) / 1024 / 1024 / 1024); }
template<typename OutputType = float> constexpr OutputType ToTiB(memory_size Size) { return OutputType(double(Size.Value) / 1024 / 1024 / 1024 / 1024); }

template<typename OutputType = float> constexpr OutputType ToKB(memory_size Size) { return OutputType(double(Size.Value) / 1000); }
template<typename OutputType = float> constexpr OutputType ToMB(memory_size Size) { return OutputType(double(Size.Value) / 1000 / 1000); }
template<typename OutputType = float> constexpr OutputType ToGB(memory_size Size) { return OutputType(double(Size.Value) / 1000 / 1000 / 1000); }
template<typename OutputType = float> constexpr OutputType ToTB(memory_size Size) { return OutputType(double(Size.Value) / 1000 / 1000 / 1000 / 1000); }



//
// ============================
//
constexpr u32  SetBit(u32 Bits, u32 Position)   { return Bits | (u32(1) << Position); }
constexpr u32  UnsetBit(u32 Bits, u32 Position) { return Bits & ~(u32(1) << Position); }
constexpr bool IsBitSet(u32 Bits, u32 Position) { return !!(Bits & (u32(1) << Position)); }

constexpr u64  SetBit(u64 Bits, u64 Position)   { return Bits | (u64(1) << Position); }
constexpr u64  UnsetBit(u64 Bits, u64 Position) { return Bits & ~(u64(1) << Position); }
constexpr bool IsBitSet(u64 Bits, u64 Position) { return !!(Bits & (u64(1) << Position)); }

constexpr bool IsPowerOfTwo(u32 Value) { return Value < 1 ? false : (Value & (Value - 1)) == 0; }
constexpr bool IsPowerOfTwo(u64 Value) { return Value < 1 ? false : (Value & (Value - 1)) == 0; }


//
// ============================
//
template<typename T = float>
constexpr T
Pi() { return (T)3.14159265359; }

template<typename T = float>
constexpr T
E() { return (T)2.71828182845; }


//
// ================
//
/// Returns the number of elements in this static array.
template<typename Type, size_t N>
constexpr size_t
Length(Type(&)[N]) { return N; }

template<typename T> struct impl_size_of { enum { SizeInBytes = sizeof(T) }; };
template<>           struct impl_size_of<void>          : impl_size_of<u8>          { };
template<>           struct impl_size_of<void const>    : impl_size_of<u8 const>    { };
template<>           struct impl_size_of<void volatile> : impl_size_of<u8 volatile> { };

/// Get the size of type T in bytes.
///
/// Same as sizeof(T) except it works also with 'void' (possibly cv-qualified)
/// where a size of 1 byte is assumed.
template<typename T>
constexpr memory_size
SizeOf() { return Bytes(impl_size_of<T>::SizeInBytes); }

/// Reinterpretation of the given pointer in case PointerType is `void`.
template<typename PointerType>
constexpr PointerType*
NonVoidPtr(PointerType* Ptr)
{
  return Ptr;
}

/// Reinterpretation of the given pointer in case PointerType is `void`.
constexpr u8*
NonVoidPtr(void* Ptr)
{
  return reinterpret_cast<u8*>(Ptr);
}

/// Reinterpretation of the given pointer in case PointerType is `void`.
constexpr u8 const*
NonVoidPtr(void const* Ptr)
{
  return reinterpret_cast<u8 const*>(Ptr);
}

/// Reinterpretation of the given pointer in case PointerType is `void`.
constexpr u8 volatile*
NonVoidPtr(void volatile* Ptr)
{
  return reinterpret_cast<u8 volatile*>(Ptr);
}

/// Advance the given pointer value by the given amount of bytes.
template<typename PointerType, typename OffsetType>
constexpr PointerType*
MemAddByteOffset(PointerType* Pointer, OffsetType Offset)
{
  return (PointerType*)((u8*)Pointer + Offset);
}

/// Advance the given pointer value by the given amount times sizeof(PointerType)
template<typename PointerType, typename OffsetType>
constexpr PointerType*
MemAddOffset(PointerType* Pointer, OffsetType Offset)
{
  return MemAddByteOffset(Pointer, Offset * ToBytes(SizeOf<PointerType>()));
}

template<typename T> struct impl_is_pod
{
  #if MTB_IsCompiler(MSVC)
    static constexpr bool Value = __is_pod(T);
  #else
    #error "IsPOD not implemented on this platform."
  #endif
};
template<> struct impl_is_pod<void>          : public impl_is_pod<u8>          {};
template<> struct impl_is_pod<void const>    : public impl_is_pod<u8 const>    {};
template<> struct impl_is_pod<void volatile> : public impl_is_pod<u8 volatile> {};

/// Whether the given type T is a "plain old data" (POD) type.
///
/// The type 'void' is also considered POD.
template<typename T>
constexpr bool
IsPOD() { return impl_is_pod<T>::Value; }


template<typename NumberType> struct impl_negate { static constexpr NumberType Do(NumberType Value) { return -Value; } };
template<> struct impl_negate<u8>  { static constexpr u8  Do(u8  Value) { return  Value; } };
template<> struct impl_negate<u16> { static constexpr u16 Do(u16 Value) { return  Value; } };
template<> struct impl_negate<u32> { static constexpr u32 Do(u32 Value) { return  Value; } };
template<> struct impl_negate<u64> { static constexpr u64 Do(u64 Value) { return  Value; } };

template<typename NumberType>
NumberType
Negate(NumberType Value)
{
  return impl_negate<NumberType>::Do(Value);
}

template<typename T>
struct impl_is_integer_type { static bool const Value = false; };
template<> struct impl_is_integer_type<i8>  { static bool const Value = true; };
template<> struct impl_is_integer_type<i16> { static bool const Value = true; };
template<> struct impl_is_integer_type<i32> { static bool const Value = true; };
template<> struct impl_is_integer_type<i64> { static bool const Value = true; };
template<> struct impl_is_integer_type<u8>  { static bool const Value = true; };
template<> struct impl_is_integer_type<u16> { static bool const Value = true; };
template<> struct impl_is_integer_type<u32> { static bool const Value = true; };
template<> struct impl_is_integer_type<u64> { static bool const Value = true; };

template<typename T>
constexpr bool
IsIntegerType() { return impl_is_integer_type<T>::Value; }

template<typename T>
struct impl_is_float_type { static bool const Value = false; };
template<> struct impl_is_float_type<float>  { static bool const Value = true; };
template<> struct impl_is_float_type<double> { static bool const Value = true; };

template<typename T>
constexpr bool
IsFloatType() { return impl_is_float_type<T>::Value; }

template<typename T>
constexpr bool
IsNumberType() { return IsFloatType<T>() || IsIntegerType<T>(); }

/// Get the number of bits of a given type.
///
/// Note: The type 'void' is not supported.
template<typename T>
constexpr size_t
NumBits() { return sizeof(T) * 8; }

template<typename T>
constexpr bool
IntIsSigned() { return ((T)-1) < 0; }

template<typename T>
constexpr T
IntMaxValue()
{
  return IntIsSigned<T>() ? (T(1) << (NumBits<T>() - 1)) - T(1)
                              : T(-1);
}

template<typename T>
constexpr T
IntMinValue()
{
  return IntIsSigned<T>() ? Negate(T(T(1) << (NumBits<T>() - 1)))
                              : T(0);
}

template<typename CharType> struct impl_is_digit_helper { static constexpr bool Do(CharType Char) { return Char >= '0' && Char <= '9'; } };
template<typename CharType> struct impl_is_digit;
template<> struct impl_is_digit<char> : public impl_is_digit_helper<char> {};

template<typename CharType>
constexpr bool
IsDigit(CharType Char)
{
  return impl_is_digit<rm_ref_const<CharType>>::Do(Char);
}

template<typename CharType>
struct impl_is_whitespace_helper
{
  static constexpr bool
  Do(CharType Char)
  {
    return Char == ' '  ||
           Char == '\n' ||
           Char == '\r' ||
           Char == '\t' ||
           Char == '\b';
  }
};

template<typename CharType> struct impl_is_whitespace;
template<> struct impl_is_whitespace<char> : public impl_is_whitespace_helper<char> {};

template<typename CharType>
constexpr bool
IsWhitespace(CharType Char)
{
  return impl_is_whitespace<rm_ref_const<CharType>>::Do(Char);
}

template<typename T> struct impl_nan;
template<> struct impl_nan<float>
{
  #if MTB_IsCompiler(MSVC)
    static constexpr float Quiet()     { return __builtin_nanf("0"); }
    static constexpr float Signaling() { return __builtin_nansf("1"); }
  #else
    #error "NaN not implemented on this platform."
  #endif
};
template<typename T> struct impl_nan;
template<> struct impl_nan<double>
{
  #if MTB_IsCompiler(MSVC)
    static constexpr double Quiet()     { return __builtin_nan("0"); }
    static constexpr double Signaling() { return __builtin_nans("1"); }
  #else
    #error "NaN not implemented on this platform."
  #endif
};

/// Returns a quiet Not-A-Number value of the given type.
template<typename T>
constexpr T
NaN()
{
  return impl_nan<T>::Quiet();
}

/// Returns a signaling Not-A-Number value of the given type.
template<typename T>
constexpr T
SignalingNaN()
{
  return impl_nan<T>::Signaling();
}

template<typename T> struct impl_is_nan;
template<> struct impl_is_nan<float>  { static constexpr bool Do(float  Value) { return Value != Value; } };
template<> struct impl_is_nan<double> { static constexpr bool Do(double Value) { return Value != Value; } };

template<typename T>
constexpr bool
IsNaN(T Value)
{
  return impl_is_nan<T>::Do(Value);
}

template<typename T> struct impl_rm_ref     { using Type = T; };
template<typename T> struct impl_rm_ref<T&> { using Type = T; };

template<typename T>
using rm_ref = typename impl_rm_ref<T>::Type;

template<typename T> struct impl_rm_const          { using Type = T; };
template<typename T> struct impl_rm_const<T const> { using Type = T; };

template<typename T>
using rm_const = typename impl_rm_const<T>::Type;

template<typename T>
using rm_ref_const = rm_const<rm_ref<T>>;

template<class Type>
constexpr typename rm_ref<Type>&&
Move(Type&& Argument)
{
  // forward Argument as movable
  return static_cast<typename rm_ref<Type>&&>(Argument);
}

template<typename Type>
constexpr Type&&
Forward(typename rm_ref<Type>& Argument)
{
  return static_cast<Type&&>(Argument);
}

template<typename Type>
constexpr Type&&
Forward(rm_ref<Type>&& Argument)
{
  return static_cast<Type&&>(Argument);
}

template<typename DestType, typename SourceType>
constexpr DestType
Cast(SourceType Value)
{
  return static_cast<DestType>(Value);
}

template<typename DestType, typename SourceType>
constexpr DestType
Reinterpret(SourceType Value)
{
  return reinterpret_cast<DestType>(Value);
}

/// Coerce value of some type to another.
///
/// Basically just a more explicit C-style cast.
template<typename DestType, typename SourceType>
DestType
Coerce(SourceType Value)
{
  DestType Result = (DestType)Value;
  return Result;
}

template<typename Type>
Type const&
AsConst(Type& Value)
{
  return const_cast<Type const&>(Value);
}

template<typename Type>
Type const*
AsPtrToConst(Type* Value)
{
  return const_cast<Type const*>(Value);
}

template<typename ToType, typename FromType>
struct impl_convert
{
  static constexpr ToType
  Do(FromType const& Value)
  {
    return Cast<ToType>(Value);
  }
};

template<typename ToType, typename FromType, typename... ExtraTypes>
ToType
Convert(FromType const& From, ExtraTypes&&... Extra)
{
  using UnqualifiedToType   = rm_ref_const<ToType>;
  using UnqualifiedFromType = rm_ref_const<FromType>;
  using Impl = impl_convert<UnqualifiedToType, UnqualifiedFromType>;
  return Impl::Do(From, Forward<ExtraTypes>(Extra)...);
}

/// Asserts on overflows and underflows when converting signed or unsigned
/// integers.
///
/// TODO: Implement what is documented below with Convert.
///
/// In case of error in non-asserting builds, this will return the
/// corresponding min / max value, instead of letting the overflow / underflow
/// happen.
template<typename DestIntegerType, typename SrcIntegerType>
inline DestIntegerType
SafeConvertInt(SrcIntegerType Value)
{
  auto Result = (DestIntegerType)Value;
  auto RevertedResult = (SrcIntegerType)Result;
  Assert(RevertedResult == Value); // Otherwise something went wrong in the conversion step (overflow/underflow).
  return Result;
}

/// \return 1 for a positive number, -1 for a negative number, 0 otherwise.
template<typename Type>
constexpr Type
Sign(Type I)
{
  return Type(I > 0 ? 1 : I < 0 ? -1 : 0);
}

template<typename T> struct impl_abs { static constexpr T Do(T Value) { return Sign(Value) * Value; } };
template<> struct impl_abs<u8>  { static constexpr u8  Do(u8  Value) { return Value; } };
template<> struct impl_abs<u16> { static constexpr u16 Do(u16 Value) { return Value; } };
template<> struct impl_abs<u32> { static constexpr u32 Do(u32 Value) { return Value; } };
template<> struct impl_abs<u64> { static constexpr u64 Do(u64 Value) { return Value; } };
template<> struct impl_abs<i8>  { static inline i8  Do(i8  Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<i16> { static inline i16 Do(i16 Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<i32> { static inline i32 Do(i32 Value) { return Value < 0 ? -Value : Value; } };
template<> struct impl_abs<i64> { static inline i64 Do(i64 Value) { return Value < 0 ? -Value : Value; } };

template<typename Type>
constexpr Type
Abs(Type Value)
{
  return impl_abs<Type>::Do(Value);
}

template<typename t_a_type, typename t_b_type>
constexpr t_a_type
Min(t_a_type A, t_b_type B)
{
  return (B < A) ? Coerce<t_a_type>(B) : A;
}

template<typename t_a_type, typename t_b_type>
constexpr t_a_type
Max(t_a_type A, t_b_type B)
{
  return (B > A) ? Coerce<t_a_type>(B) : A;
}

template<typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_value_type
Clamp(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return UpperBound < LowerBound ? Value : Min(UpperBound, Max(LowerBound, Value));
}

// TODO: Make this a constexpr?
template<typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
t_value_type
Wrap(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  const auto BoundsDelta = (Coerce<t_lower_bound_type>(UpperBound) - LowerBound);
  while(Value >= UpperBound) Value -= BoundsDelta;
  while(Value < LowerBound)  Value += BoundsDelta;
  return Value;
  // return Value >= UpperBound ? Value - BoundsDelta :
  //        Value <  LowerBound ? Value + BoundsDelta :
  //                              Value;
}

double
Pow(double Base, double Exponent);

float
Pow(float Base, float Exponent);

template<typename ReturnType = double, typename BaseType, typename ExponentType>
constexpr ReturnType
Pow(BaseType Base, ExponentType Exponent) { return (ReturnType)Pow((double)Base, (double)Exponent); }

double
Mod(double Value, double Divisor);

float
Mod(float Value, float Divisor);

double
Sqrt(double Value);

float
Sqrt(float Value);

template<typename ReturnType = double, typename T>
constexpr ReturnType
Sqrt(T Value) { return (ReturnType)Sqrt((double)Value); }

float
InvSqrt(float Value);

//
// RoundDown
//
template<typename OutputType, typename InputType>
struct impl_round_down;

template<typename OutputType>
struct impl_round_down<OutputType, float>
{
  static inline OutputType
  Do(float Value)
  {
    return Convert<OutputType>(::floorf(Value));
  }
};

template<typename OutputType>
struct impl_round_down<OutputType, double>
{
  static inline OutputType
  Do(double Value)
  {
    return Convert<OutputType>(::floor(Value));
  }
};

template<typename OutputType, typename InputType>
struct impl_round_down : public impl_round_down<OutputType, double> {};

/// Also known as the floor-function.
template<typename OutputType, typename InputType>
inline OutputType
RoundDown(InputType Value)
{
  return impl_round_down<OutputType, InputType>::Do(Value);
}

//
// RoundUp
//
template<typename OutputType, typename InputType>
struct impl_round_up;

template<typename OutputType>
struct impl_round_up<OutputType, float>
{
  static inline OutputType
  Do(float Value)
  {
    return Convert<OutputType>(::ceilf(Value));
  }
};

template<typename OutputType>
struct impl_round_up<OutputType, double>
{
  static inline OutputType
  Do(double Value)
  {
    return Convert<OutputType>(::ceil(Value));
  }
};

template<typename OutputType, typename InputType>
struct impl_round_up : public impl_round_up<OutputType, double> {};

/// Also known as the ceil-function.
template<typename OutputType, typename InputType>
inline OutputType
RoundUp(InputType Value)
{
  return impl_round_up<OutputType, InputType>::Do(Value);
}


//
// RoundTowardsZero
//
template<typename OutputType, typename InputType> struct impl_round_towards_zero
{
  static inline OutputType
  Do(InputType Value)
  {
    return Value > 0 ? RoundDown<OutputType>(Value) : RoundUp<OutputType>(Value);
  }
};

/// Round towards zero.
///
/// Equivalent to \code Value > 0 ? RoundDown(Value) : RoundUp(Value) \endcode
template<typename OutputType, typename InputType>
inline OutputType
RoundTowardsZero(InputType Value)
{
  return impl_round_towards_zero<OutputType, InputType>::Do(Value);
}


//
// RoundAwayFromZero
//
template<typename OutputType, typename InputType> struct impl_round_away_from_zero
{
  static inline OutputType
  Do(InputType Value)
  {
    return Value > 0 ? RoundUp<OutputType>(Value) : RoundDown<OutputType>(Value);
  }
};

/// Round away from zero.
///
/// Equivalent to \code Value > 0 ? RoundUp(Value) : RoundDown(Value) \endcode
template<typename OutputType, typename InputType>
inline OutputType
RoundAwayFromZero(InputType Value)
{
  return impl_round_away_from_zero<OutputType, InputType>::Do(Value);
}


//
// Round
//
template<typename OutputType, typename InputType> struct impl_round
{
  static inline OutputType
  Do(InputType Value)
  {
    return RoundDown<OutputType>(Value + InputType(0.5f));
  }
};

/// Round to the nearest integral value.
template<typename OutputType, typename InputType>
inline OutputType
Round(InputType Value)
{
  return impl_round<OutputType, InputType>::Do(Value);
}


// Project a value from [LowerBound, UpperBound] to [0, 1]
// Example:
//   auto Result = NormalizeValue<float>(15, 10, 30); // == 0.25f
template<typename t_result, typename t_value_type, typename t_lower_bound_type, typename t_upper_bound_type>
constexpr t_result
NormalizeValue(t_value_type Value, t_lower_bound_type LowerBound, t_upper_bound_type UpperBound)
{
  return UpperBound <= LowerBound ?
         t_result(0) : // Bogus bounds.
         Cast<t_result>(Value - LowerBound) / Cast<t_result>(UpperBound - LowerBound);
}

bool
AreNearlyEqual(double A, double B, double Epsilon = 1e-4);

bool
AreNearlyEqual(float A, float B, float Epsilon = 1e-4f);

inline bool
IsNearlyZero(double A, double Epsilon = 1e-4) { return AreNearlyEqual(A, 0, Epsilon); }

inline bool
IsNearlyZero(float A, float Epsilon = 1e-4f) { return AreNearlyEqual(A, 0, Epsilon); }

template<typename t_a, typename t_b>
inline void
Swap(t_a& A, t_b& B)
{
  auto Temp{ Move(A) };
  A = Move(B);
  B = Move(Temp);
}

/// Maps the given float Value from [0, 1] to [0, MaxValueOf(UNormType)]
template<typename UNormType>
UNormType constexpr
FloatToUNorm(float Value)
{
  return Cast<UNormType>(Clamp((Value * IntMaxValue<UNormType>()) + 0.5f, 0.0f, IntMaxValue<UNormType>()));
}

/// Maps the given unsigned byte Value from [0, 255] to [0, 1]
template<typename UNormType>
float constexpr
UNormToFloat(UNormType Value)
{
  return Clamp(Cast<float>(Value) / IntMaxValue<UNormType>(), 0.0f, 1.0f);
}

/// \see InitStruct
template<typename T>
struct impl_init_struct
{
  // Return an initialized instance of T.
  template<typename... ArgTypes>
  static constexpr T
  Create(ArgTypes&&... Args) { return { Forward<ArgTypes>(Args)... }; }
};

/// Utility function to initialize a struct of the given type with a chance
/// for centralized specialization.
///
/// To control the default or non-default construction behavior of a certain
/// struct the template \c impl_init_struct can be specialized and a Create()
/// function must be provided.
///
/// \see impl_init_struct
template<typename T, typename... ArgTypes>
inline auto
InitStruct(ArgTypes&&... Args)
  -> decltype(impl_init_struct<rm_ref<T>>::Create(Forward<ArgTypes>(Args)...))
{
  // Note: specializations for impl_init_struct are found in VulkanHelper.inl
  return impl_init_struct<rm_ref<T>>::Create(Forward<ArgTypes>(Args)...);
}

struct impl_defer
{
  template<typename LambdaType>
  struct defer
  {
    LambdaType Lambda;
    defer(LambdaType InLambda) : Lambda{ Move(InLambda) } {}
    ~defer() { Lambda(); }
  };

  template<typename t_in_func_type>
  defer<t_in_func_type> operator =(t_in_func_type InLambda) { return { Move(InLambda) }; }
};

/// Defers execution of code until the end of the current scope.
///
/// Usage:
///   int i = 0;
///   MTB_Defer [&](){ i++; printf("Foo %d\n", i); };
///   MTB_Defer [&](){ i++; printf("Bar %d\n", i); };
///   MTB_Defer [=](){      printf("Baz %d\n", i); };
///
/// Output:
///   Baz 0
///   Bar 1
///   Foo 2
///
/// \param CaptureSpec The lambda capture specification.
#define MTB_Defer auto MTB_Concat(_Defer, MTB_Line) = ::mtb::impl_defer() =

} // namespace mtb

#endif // !defined(MTB_HEADER_mtb_common)


//
// Implementation
//

#if defined(MTB_COMMON_IMPLEMENTATION)

// Impl guard.
#if !defined(MTB_IMPL_mtb_common)
#define MTB_IMPL_mtb_common

#include <math.h>

void mtb::
operator +=(memory_size& A, memory_size B)
{
  A.Value += B.Value;
}

void mtb::
operator -=(memory_size& A, memory_size B)
{
  A.Value -= B.Value;
}

void mtb::
operator *=(memory_size& A, u64 Scale)
{
  A.Value *= Scale;
}

void mtb::
operator /=(memory_size& A, u64 Scale)
{
  A.Value /= Scale;
}

float mtb::
Pow(float Base, float Exponent)
{
  return powf(Base, Exponent);
}

double mtb::
Pow(double Base, double Exponent)
{
  return pow(Base, Exponent);
}

float mtb::
Mod(float Value, float Divisor)
{
  return fmodf(Value, Divisor);
}

double mtb::
Mod(double Value, double Divisor)
{
  return fmod(Value, Divisor);
}

float mtb::
Sqrt(float Value)
{
  return sqrtf(Value);
}

double mtb::
Sqrt(double Value)
{
  return sqrt(Value);
}

float mtb::
InvSqrt(float Value)
{
  union FloatInt
  {
    float Float;
    int Int;
  };
  FloatInt MagicNumber;
  float HalfValue;
  float Result;
  const float ThreeHalfs = 1.5f;

  HalfValue = Value * 0.5f;
  Result = Value;
  MagicNumber.Float = Result;                               // evil floating point bit level hacking
  MagicNumber.Int  = 0x5f3759df - ( MagicNumber.Int >> 1 ); // what the fuck?
  Result = MagicNumber.Float;
  Result = Result * ( ThreeHalfs - ( HalfValue * Result * Result ) ); // 1st iteration

  return Result;
}

bool mtb::
AreNearlyEqual(double A, double B, double Epsilon)
{
  return Abs(A - B) <= Epsilon;
}

bool mtb::
AreNearlyEqual(float A, float B, float Epsilon)
{
  return Abs(A - B) <= Epsilon;
}

#endif // !defined(MTB_IMPL_mtb_common)
#endif // defined(MTB_COMMON_IMPLEMENTATION)
