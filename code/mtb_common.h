#if !defined(MTB_HEADER_mtb_common)
#define MTB_HEADER_mtb_common

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_COMMON_IMPLEMENTATION)
  #define MTB_COMMON_IMPLEMENTATION
#endif

#if defined(MTB_COMMON_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_platform.h"
#include "mtb_assert.h"

//
// Primitive array types.
//

/// Defines some array variants of types for better readability when used as
/// function parameters.
///
/// For example, a function `Foo` that accepts and array of 4 floats by
/// reference-to-const looks like this:
/// \code
/// void Foo(mtb_f32 const (&ParamName)[4]);
/// \endcode
///
/// Using these typedefs, this can be transformed into:
/// \code
/// void Foo(mtb_f32_4 const& ParamName);
/// \endcode
#define MTB_DEFINE_ARRAY_TYPES(TheType)\
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

MTB_DEFINE_ARRAY_TYPES(mtb_s08);   // mtb_s08   => mtb_s08_1,   mtb_s08_2,   ..., mtb_s08_4x3,   mtb_s08_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_s16);   // mtb_s16   => mtb_s16_1,   mtb_s16_2,   ..., mtb_s16_4x3,   mtb_s16_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_s32);   // mtb_s32   => mtb_s32_1,   mtb_s32_2,   ..., mtb_s32_4x3,   mtb_s32_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_s64);   // mtb_s64   => mtb_s64_1,   mtb_s64_2,   ..., mtb_s64_4x3,   mtb_s64_4x4

MTB_DEFINE_ARRAY_TYPES(mtb_u08);  // mtb_u08  => mtb_u08_1,  mtb_u08_2,  ..., mtb_u08_4x3,  mtb_u08_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_u16);  // mtb_u16  => mtb_u16_1,  mtb_u16_2,  ..., mtb_u16_4x3,  mtb_u16_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_u32);  // mtb_u32  => mtb_u32_1,  mtb_u32_2,  ..., mtb_u32_4x3,  mtb_u32_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_u64);  // mtb_u64  => mtb_u64_1,  mtb_u64_2,  ..., mtb_u64_4x3,  mtb_u64_4x4

MTB_DEFINE_ARRAY_TYPES(mtb_f32); // mtb_f32 => mtb_f32_1, mtb_f32_2, ..., mtb_f32_4x3, mtb_f32_4x4
MTB_DEFINE_ARRAY_TYPES(mtb_f64); // mtb_f64 => mtb_f64_1, mtb_f64_2, ..., mtb_f64_4x3, mtb_f64_4x4


//
// ============================
//

constexpr mtb_u64 mtb_KiB(mtb_u64 Amount) { return { Amount * 1024 }; }
constexpr mtb_u64 mtb_MiB(mtb_u64 Amount) { return { Amount * 1024 * 1024 }; }
constexpr mtb_u64 mtb_GiB(mtb_u64 Amount) { return { Amount * 1024 * 1024 * 1024 }; }
constexpr mtb_u64 mtb_TiB(mtb_u64 Amount) { return { Amount * 1024 * 1024 * 1024 * 1024 }; }

constexpr mtb_u64 mtb_KB(mtb_u64 Amount) { return { Amount * 1000 }; }
constexpr mtb_u64 mtb_MB(mtb_u64 Amount) { return { Amount * 1000 * 1000 }; }
constexpr mtb_u64 mtb_GB(mtb_u64 Amount) { return { Amount * 1000 * 1000 * 1000 }; }
constexpr mtb_u64 mtb_TB(mtb_u64 Amount) { return { Amount * 1000 * 1000 * 1000 * 1000 }; }

constexpr mtb_f64 mtb_ToKiB(mtb_u64 Size) { return (mtb_f64)Size / 1024; }
constexpr mtb_f64 mtb_ToMiB(mtb_u64 Size) { return (mtb_f64)Size / 1024 / 1024; }
constexpr mtb_f64 mtb_ToGiB(mtb_u64 Size) { return (mtb_f64)Size / 1024 / 1024 / 1024; }
constexpr mtb_f64 mtb_ToTiB(mtb_u64 Size) { return (mtb_f64)Size / 1024 / 1024 / 1024 / 1024; }

constexpr mtb_f64 mtb_ToKB(mtb_u64 Size) { return (mtb_f64)Size / 1000; }
constexpr mtb_f64 mtb_ToMB(mtb_u64 Size) { return (mtb_f64)Size / 1000 / 1000; }
constexpr mtb_f64 mtb_ToGB(mtb_u64 Size) { return (mtb_f64)Size / 1000 / 1000 / 1000; }
constexpr mtb_f64 mtb_ToTB(mtb_u64 Size) { return (mtb_f64)Size / 1000 / 1000 / 1000 / 1000; }



//
// ============================
//
constexpr mtb_u32 mtb_SetBit(mtb_u32 Bits, mtb_u32 Position)   { return Bits | (mtb_u32(1) << Position); }
constexpr mtb_u32 mtb_UnsetBit(mtb_u32 Bits, mtb_u32 Position) { return Bits & ~(mtb_u32(1) << Position); }
constexpr bool mtb_IsBitSet(mtb_u32 Bits, mtb_u32 Position) { return !!(Bits & (mtb_u32(1) << Position)); }

constexpr mtb_u64 mtb_SetBit(mtb_u64 Bits, mtb_u64 Position)   { return Bits | (mtb_u64(1) << Position); }
constexpr mtb_u64 mtb_UnsetBit(mtb_u64 Bits, mtb_u64 Position) { return Bits & ~(mtb_u64(1) << Position); }
constexpr bool mtb_IsBitSet(mtb_u64 Bits, mtb_u64 Position) { return !!(Bits & (mtb_u64(1) << Position)); }

constexpr bool mtb_IsPowerOfTwo(mtb_u32 Value) { return Value < 1 ? false : (Value & (Value - 1)) == 0; }
constexpr bool mtb_IsPowerOfTwo(mtb_u64 Value) { return Value < 1 ? false : (Value & (Value - 1)) == 0; }

constexpr mtb_u32 mtb_AlignValue_PowerOfTwo(mtb_u32 Value, int Alignment) { return ((Value + Alignment - 1) / Alignment) * Alignment; }
constexpr mtb_u64 mtb_AlignValue_PowerOfTwo(mtb_u64 Value, int Alignment) { return ((Value + Alignment - 1) / Alignment) * Alignment; }

constexpr void* mtb_AlignPointer(void* Pointer, int Alignment) { return (void*)mtb_AlignValue_PowerOfTwo((size_t)Pointer, Alignment); }

//
// ============================
//
#define MTB_Pi32     3.14159265359f
#define MTB_HalfPi32 1.57079632679f
#define MTB_InvPi32  0.31830988618f

#define MTB_Pi64     3.14159265359
#define MTB_HalfPi64 1.57079632679
#define MTB_InvPi64  0.31830988618

#define MTB_E32     2.71828182845f
#define MTB_HalfE32 1.35914091423f
#define MTB_InvE32  0.36787944117f

#define MTB_E64     2.71828182845
#define MTB_HalfE64 1.35914091423
#define MTB_InvE64  0.36787944117


//
// ================
//
template<typename T> struct mtb_impl_size_of { enum { SizeInBytes = sizeof(T) }; };
template<>           struct mtb_impl_size_of<void>          : mtb_impl_size_of<mtb_byte>          { };
template<>           struct mtb_impl_size_of<void const>    : mtb_impl_size_of<mtb_byte const>    { };
template<>           struct mtb_impl_size_of<void volatile> : mtb_impl_size_of<mtb_byte volatile> { };

/// Get the size of type T in bytes.
///
/// Same as sizeof(T) except it works also with (cv-qualified) 'void' where a
/// size of 1 byte is assumed.
template<typename T>
constexpr size_t
mtb_SafeSizeOf() { return (size_t)mtb_impl_size_of<T>::SizeInBytes; }

/// Returns the number of elements in this static array.
template<typename T, size_t N>
constexpr size_t
mtb_ArrayLengthOf(T(&)[N]) { return N; }

/// Returns the size in bytes that the given array occupies.
template<typename T, size_t N>
constexpr size_t
mtb_ArrayByteSizeOf(T(&)[N]) { return N * sizeof(T); }

/// Get length of the given \a String, not including the trailing zero.
MTB_INLINE size_t
mtb_StringByteSizeOf(char const* String)
{
  size_t Result = 0;
  while(*String++) { ++Result; }
  return Result;
}

/// Get length of the given \a String, not including the trailing zero.
MTB_INLINE size_t
mtb_StringLengthOf(char const* String)
{
  // TODO: UTF-8
  return mtb_StringByteSizeOf(String);
}

MTB_INLINE int
mtb_StringCompare(size_t Len, char const* PtrA, char const* PtrB)
{
  int Result = 0;

  for(size_t ByteIndex = 0; ByteIndex < Len; ++ByteIndex)
  {
    if(PtrA[ByteIndex] != PtrB[ByteIndex])
    {
      Result = PtrA[ByteIndex] < PtrB[ByteIndex] ? -1 : 1;
      break;
    }
  }

  return Result;
}

MTB_INLINE int
mtb_StringCompare(size_t LenA, char const* PtrA, size_t LenB, char const* PtrB)
{
  int Result = 0;

  if(LenA == LenB) { Result = mtb_StringCompare(LenA, PtrA, PtrB); }
  else             { Result = LenA < LenB ? -1 : 1; }

  return Result;
}

MTB_INLINE bool
mtb_StringsAreEqual(size_t LenA, char const* PtrA, size_t LenB, char const* PtrB)
{
  bool Result = mtb_StringCompare(LenA, PtrA, LenB, PtrB) == 0;
  return Result;
}

MTB_INLINE bool
mtb_StringsAreEqual(size_t Len, char const* PtrA, char const* PtrB)
{
  bool Result = mtb_StringCompare(Len, PtrA, PtrB) == 0;
  return Result;
}

MTB_INLINE bool
mtb_StringsAreEqual(char const* PtrA, char const* PtrB)
{
  size_t LenA = mtb_StringLengthOf(PtrA);
  size_t LenB = mtb_StringLengthOf(PtrB);
  bool Result = mtb_StringCompare(LenA, PtrA, LenB, PtrB) == 0;
  return Result;
}

struct mtb_concat_strings_result
{
  size_t Len;
  char* Ptr;
};
/// Concatenate two strings of the given sizes together.
///
/// \return The result is pointing into the given buffer memory.
mtb_concat_strings_result
mtb_ConcatStrings(size_t HeadSize, char const* HeadPtr, size_t TailSize, char const* TailPtr, size_t BufferSize, char* BufferPtr);

/// Concatenates two zero-terminated strings together using the given buffer.
MTB_INLINE mtb_concat_strings_result
mtb_ConcatStrings(char const* HeadPtr, char const* TailPtr, size_t BufferSize, char* BufferPtr)
{
  size_t HeadSize = mtb_StringLengthOf(HeadPtr);
  size_t TailSize = mtb_StringLengthOf(TailPtr);
  mtb_concat_strings_result Result = mtb_ConcatStrings(HeadSize, HeadPtr, TailSize, TailPtr, BufferSize, BufferPtr);
  return Result;
}

//
// MTB_IsStrictPod
#if MTB_CURRENT_COMPILER == MTB_COMPILER_TYPE_MSVC
  #define MTB_IsStrictPod(TYPE) __is_pod(TYPE)
#else
  #error "MTB_IsStrictPod() not implemented for this compiler."
#endif


//
// MTB_IsPod
template<typename T>
struct mtb_impl_is_pod { static constexpr bool Value = MTB_IsStrictPod(T); };
template<> struct mtb_impl_is_pod<void>          { static constexpr bool Value = true; };
template<> struct mtb_impl_is_pod<void const>    { static constexpr bool Value = true; };
template<> struct mtb_impl_is_pod<void volatile> { static constexpr bool Value = true; };

#define MTB_IsPod(TYPE) mtb_impl_is_pod<TYPE>::Value

//
// MTB_(Min|Max)Value_*
#define MTB_MinValue_s08 ((mtb_s08)0x7F)
#define MTB_MaxValue_s08 ((mtb_s08)0x80)
#define MTB_MinValue_s16 ((mtb_s16)0x7FFF)
#define MTB_MaxValue_s16 ((mtb_s16)0x8000)
#define MTB_MinValue_s32 ((mtb_s32)0x7FFFFFFF)
#define MTB_MaxValue_s32 ((mtb_s32)0x80000000)
#define MTB_MinValue_s64 ((mtb_s64)0x7FFFFFFFFFFFFFFF)
#define MTB_MaxValue_s64 ((mtb_s64)0x8000000000000000)

#define MTB_MinValue_u08 ((mtb_u08)0)
#define MTB_MaxValue_u08 ((mtb_u08)0xFF)
#define MTB_MinValue_u16 ((mtb_u16)0)
#define MTB_MaxValue_u16 ((mtb_u16)0xFFFF)
#define MTB_MinValue_u32 ((mtb_u32)0)
#define MTB_MaxValue_u32 ((mtb_u32)0xFFFFFFFF)
#define MTB_MinValue_u64 ((mtb_u64)0)
#define MTB_MaxValue_u64 ((mtb_u64)0xFFFFFFFFFFFFFFFF)


#define MTB_GetNumBits(TYPE) (sizeof(TYPE)*8)


constexpr bool mtb_IsDigit(char Value) { return Value >= '0' && Value <= '9'; }
constexpr bool mtb_IsWhitespace(char Value) { return Value == ' '  || Value == '\n' || Value == '\r' || Value == '\t' || Value == '\b'; }

//
// NaN
#if MTB_CURRENT_COMPILER == MTB_COMPILER_TYPE_MSVC
  constexpr mtb_f32 mtb_Impl_QNaN_f32() { return __builtin_nanf("0"); }
  constexpr mtb_f64 mtb_Impl_QNaN_f64() { return __builtin_nanf("0"); }

  #define MTB_QuietNaN_f32 mtb_Impl_QNaN_f32()
  #define MTB_QuietNaN_f64 mtb_Impl_QNaN_f64()

  constexpr mtb_f32 mtb_Impl_SNaN_f32() { return __builtin_nanf("0"); }
  constexpr mtb_f64 mtb_Impl_SNaN_f64() { return __builtin_nanf("0"); }

  #define MTB_SignallingNaN_f32 mtb_Impl_SNaN_f32()
  #define MTB_SignallingNaN_f64 mtb_Impl_SNaN_f64()

  #define MTB_NaN_f32 MTB_QuietNaN_f32
  #define MTB_NaN_f64 MTB_QuietNaN_f64
#else
  #error "MTB_*NaN* not implemented on this platform."
#endif

constexpr bool mtb_IsNaN(mtb_f32 Value) { return Value != Value; }
constexpr bool mtb_IsNaN(mtb_f64 Value) { return Value != Value; }


template<typename T> struct mtb_impl_rm_ref     { using Type = T; };
template<typename T> struct mtb_impl_rm_ref<T&> { using Type = T; };

template<typename T>
using mtb_rm_ref = typename mtb_impl_rm_ref<T>::Type;

template<typename T> struct mtb_impl_rm_const          { using Type = T; };
template<typename T> struct mtb_impl_rm_const<T const> { using Type = T; };

template<typename T>
using mtb_rm_const = typename mtb_impl_rm_const<T>::Type;

template<typename T>
using mtb_rm_ref_const = mtb_rm_const<mtb_rm_ref<T>>;

template<class T>
constexpr typename mtb_rm_ref<T>&&
mtb_Move(T&& Argument)
{
  // forward Argument as movable
  return static_cast<typename mtb_rm_ref<T>&&>(Argument);
}

template<typename T>
constexpr T&&
mtb_Forward(typename mtb_rm_ref<T>& Argument)
{
  return static_cast<T&&>(Argument);
}

template<typename T>
constexpr T&&
mtb_Forward(mtb_rm_ref<T>&& Argument)
{
  return static_cast<T&&>(Argument);
}

template<typename DestT, typename SourceT>
struct mtb_impl_convert
{
  static constexpr DestT
  Do(SourceT const& Value)
  {
    return Cast<DestT>(Value);
  }
};

template<typename DestT, typename SourceT, typename... ExtraTypes>
decltype(auto)
mtb_Convert(SourceT const& ToConvert, ExtraTypes&&... Extra)
{
  using UnqualifiedDestT   = mtb_rm_ref_const<DestT>;
  using UnqualifiedSourceT = mtb_rm_ref_const<SourceT>;
  using Impl = mtb_impl_convert<UnqualifiedDestT, UnqualifiedSourceT>;
  return Impl::Do(ToConvert, mtb_Forward<ExtraTypes>(Extra)...);
}

#define MTB__SAFE_CONVERT_BODY(TYPE) if(Value >= MTB_MinValue_##TYPE && Value <= MTB_MaxValue_##TYPE) \
  MTB_Require(Value >= MTB_MinValue_##TYPE && Value <= MTB_MaxValue_##TYPE, "Value out of range."); \
  mtb_##TYPE Result = (mtb_##TYPE)Value; \
  return Result;

MTB_INLINE mtb_s08 mtb_SafeConvert_s08(mtb_s16 Value) { MTB__SAFE_CONVERT_BODY(s08); }
MTB_INLINE mtb_s08 mtb_SafeConvert_s08(mtb_s32 Value) { MTB__SAFE_CONVERT_BODY(s08); }
MTB_INLINE mtb_s08 mtb_SafeConvert_s08(mtb_s64 Value) { MTB__SAFE_CONVERT_BODY(s08); }
MTB_INLINE mtb_s16 mtb_SafeConvert_s16(mtb_s32 Value) { MTB__SAFE_CONVERT_BODY(s16); }
MTB_INLINE mtb_s16 mtb_SafeConvert_s16(mtb_s64 Value) { MTB__SAFE_CONVERT_BODY(s16); }
MTB_INLINE mtb_s32 mtb_SafeConvert_s32(mtb_s64 Value) { MTB__SAFE_CONVERT_BODY(s32); }

MTB_INLINE mtb_u08 mtb_SafeConvert_u08(mtb_u16 Value) { MTB__SAFE_CONVERT_BODY(u08); }
MTB_INLINE mtb_u08 mtb_SafeConvert_u08(mtb_u32 Value) { MTB__SAFE_CONVERT_BODY(u08); }
MTB_INLINE mtb_u08 mtb_SafeConvert_u08(mtb_u64 Value) { MTB__SAFE_CONVERT_BODY(u08); }
MTB_INLINE mtb_u16 mtb_SafeConvert_u16(mtb_u32 Value) { MTB__SAFE_CONVERT_BODY(u16); }
MTB_INLINE mtb_u16 mtb_SafeConvert_u16(mtb_u64 Value) { MTB__SAFE_CONVERT_BODY(u16); }
MTB_INLINE mtb_u32 mtb_SafeConvert_u32(mtb_u64 Value) { MTB__SAFE_CONVERT_BODY(u32); }


constexpr mtb_s08 mtb_Sign(mtb_s08 Value) { return (mtb_s08)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }
constexpr mtb_s16 mtb_Sign(mtb_s16 Value) { return (mtb_s16)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }
constexpr mtb_s32 mtb_Sign(mtb_s32 Value) { return (mtb_s32)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }
constexpr mtb_s64 mtb_Sign(mtb_s64 Value) { return (mtb_s64)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }
constexpr mtb_f32 mtb_Sign(mtb_f32 Value) { return (mtb_f32)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }
constexpr mtb_f64 mtb_Sign(mtb_f64 Value) { return (mtb_f64)(Value > 0 ? 1 : Value < 0 ? -1 : 0); }


constexpr mtb_s08 mtb_Abs(mtb_s08 Value) { return mtb_Sign(Value)*Value; }
constexpr mtb_s16 mtb_Abs(mtb_s16 Value) { return mtb_Sign(Value)*Value; }
constexpr mtb_s32 mtb_Abs(mtb_s32 Value) { return mtb_Sign(Value)*Value; }
constexpr mtb_s64 mtb_Abs(mtb_s64 Value) { return mtb_Sign(Value)*Value; }
constexpr mtb_f32 mtb_Abs(mtb_f32 Value) { return mtb_Sign(Value)*Value; }
constexpr mtb_f64 mtb_Abs(mtb_f64 Value) { return mtb_Sign(Value)*Value; }


constexpr mtb_s08 mtb_Min(mtb_s08 A, mtb_s08 B) { return A < B ? A : B; }
constexpr mtb_s16 mtb_Min(mtb_s16 A, mtb_s16 B) { return A < B ? A : B; }
constexpr mtb_s32 mtb_Min(mtb_s32 A, mtb_s32 B) { return A < B ? A : B; }
constexpr mtb_s64 mtb_Min(mtb_s64 A, mtb_s64 B) { return A < B ? A : B; }
constexpr mtb_u08 mtb_Min(mtb_u08 A, mtb_u08 B) { return A < B ? A : B; }
constexpr mtb_u16 mtb_Min(mtb_u16 A, mtb_u16 B) { return A < B ? A : B; }
constexpr mtb_u32 mtb_Min(mtb_u32 A, mtb_u32 B) { return A < B ? A : B; }
constexpr mtb_u64 mtb_Min(mtb_u64 A, mtb_u64 B) { return A < B ? A : B; }
constexpr mtb_f32 mtb_Min(mtb_f32 A, mtb_f32 B) { return A < B ? A : B; }
constexpr mtb_f64 mtb_Min(mtb_f64 A, mtb_f64 B) { return A < B ? A : B; }

constexpr mtb_s08 mtb_Max(mtb_s08 A, mtb_s08 B) { return A > B ? A : B; }
constexpr mtb_s16 mtb_Max(mtb_s16 A, mtb_s16 B) { return A > B ? A : B; }
constexpr mtb_s32 mtb_Max(mtb_s32 A, mtb_s32 B) { return A > B ? A : B; }
constexpr mtb_s64 mtb_Max(mtb_s64 A, mtb_s64 B) { return A > B ? A : B; }
constexpr mtb_u08 mtb_Max(mtb_u08 A, mtb_u08 B) { return A > B ? A : B; }
constexpr mtb_u16 mtb_Max(mtb_u16 A, mtb_u16 B) { return A > B ? A : B; }
constexpr mtb_u32 mtb_Max(mtb_u32 A, mtb_u32 B) { return A > B ? A : B; }
constexpr mtb_u64 mtb_Max(mtb_u64 A, mtb_u64 B) { return A > B ? A : B; }
constexpr mtb_f32 mtb_Max(mtb_f32 A, mtb_f32 B) { return A > B ? A : B; }
constexpr mtb_f64 mtb_Max(mtb_f64 A, mtb_f64 B) { return A > B ? A : B; }

constexpr mtb_s08 mtb_Clamp(mtb_s08 Value, mtb_s08 Lower, mtb_s08 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_s16 mtb_Clamp(mtb_s16 Value, mtb_s16 Lower, mtb_s16 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_s32 mtb_Clamp(mtb_s32 Value, mtb_s32 Lower, mtb_s32 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_s64 mtb_Clamp(mtb_s64 Value, mtb_s64 Lower, mtb_s64 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_u08 mtb_Clamp(mtb_u08 Value, mtb_u08 Lower, mtb_u08 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_u16 mtb_Clamp(mtb_u16 Value, mtb_u16 Lower, mtb_u16 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_u32 mtb_Clamp(mtb_u32 Value, mtb_u32 Lower, mtb_u32 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_u64 mtb_Clamp(mtb_u64 Value, mtb_u64 Lower, mtb_u64 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_f32 mtb_Clamp(mtb_f32 Value, mtb_f32 Lower, mtb_f32 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }
constexpr mtb_f64 mtb_Clamp(mtb_f64 Value, mtb_f64 Lower, mtb_f64 Upper) { return Upper < Lower ? Value : mtb_Min(Upper, mtb_Max(Lower, Value)); }


mtb_f64 mtb_Pow(mtb_f64 Base, mtb_f64 Exponent);
mtb_f32 mtb_Pow(mtb_f32 Base, mtb_f32 Exponent);

mtb_f64 mtb_Mod(mtb_f64 Value, mtb_f64 Divisor);
mtb_f32 mtb_Mod(mtb_f32 Value, mtb_f32 Divisor);

mtb_f64 mtb_Sqrt(mtb_f64 Value);
mtb_f32 mtb_Sqrt(mtb_f32 Value);

mtb_f32 mtb_InvSqrt(mtb_f32 Value);

//
// Rounding
//
mtb_f32 mtb_RoundDown(mtb_f32 Value);
mtb_f64 mtb_RoundDown(mtb_f64 Value);

mtb_f32 mtb_RoundUp(mtb_f32 Value);
mtb_f64 mtb_RoundUp(mtb_f64 Value);

MTB_INLINE mtb_f32 mtb_Round(mtb_f32 Value) { return mtb_RoundDown(Value + 0.5f); }
MTB_INLINE mtb_f64 mtb_Round(mtb_f64 Value) { return mtb_RoundDown(Value + 0.5); }

MTB_INLINE mtb_f32 mtb_RoundTowardsZero(mtb_f32 Value) { return Value < 0 ? mtb_RoundUp(Value) : mtb_RoundDown(Value); }
MTB_INLINE mtb_f64 mtb_RoundTowardsZero(mtb_f64 Value) { return Value < 0 ? mtb_RoundUp(Value) : mtb_RoundDown(Value); }

MTB_INLINE mtb_f32 mtb_RoundAwayFromZero(mtb_f32 Value) { return Value < 0 ? mtb_RoundDown(Value) : mtb_RoundUp(Value); }
MTB_INLINE mtb_f64 mtb_RoundAwayFromZero(mtb_f64 Value) { return Value < 0 ? mtb_RoundDown(Value) : mtb_RoundUp(Value); }


// Project a value from [LowerBound, UpperBound] to [0, 1]
// Example:
//   auto Result = mtb_NormalizeValue<mtb_f32>(15, 10, 30); // == 0.25f
// TODO: Un-templatize
template<typename ResultT, typename ValueT, typename LowerBoundT, typename UpperBoundT>
constexpr ResultT
mtb_NormalizeValue(ValueT Value, LowerBoundT LowerBound, UpperBoundT UpperBound)
{
  return UpperBound <= LowerBound ?
         ResultT(0) : // Bogus bounds.
         (ResultT)(Value - LowerBound) / (ResultT)(UpperBound - LowerBound);
}

constexpr mtb_f32
mtb_Lerp(mtb_f32 A, mtb_f32 B, mtb_f32 Alpha)
{
  return (1.0f - Alpha) * A + Alpha * B;
}

constexpr mtb_f64
mtb_Lerp(mtb_f64 A, mtb_f64 B, mtb_f64 Alpha)
{
  return (1.0 - Alpha) * A + Alpha * B;
}

bool
mtb_AreNearlyEqual(mtb_f32 A, mtb_f32 B, mtb_f32 Epsilon = 1e-4f);

bool
mtb_AreNearlyEqual(mtb_f64 A, mtb_f64 B, mtb_f64 Epsilon = 1e-4);

MTB_INLINE bool
mtb_IsNearlyZero(mtb_f32 A, mtb_f32 Epsilon = 1e-4f) { return mtb_AreNearlyEqual(A, 0, Epsilon); }

MTB_INLINE bool
mtb_IsNearlyZero(mtb_f64 A, mtb_f64 Epsilon = 1e-4) { return mtb_AreNearlyEqual(A, 0, Epsilon); }

template<typename TypeA, typename TypeB>
MTB_INLINE void
mtb_Swap(TypeA& A, TypeB& B)
{
  auto Temp{ mtb_Move(A) };
  A = mtb_Move(B);
  B = mtb_Move(Temp);
}

template<typename T, typename U = T>
MTB_INLINE T
mtb_Exchange(T& Value, U&& NewValue)
{
  auto OldValue{ mtb_Move(Value) };
  Value = mtb_Forward<T>(NewValue);
  return OldValue;
}

/// Maps the given mtb_f32 Value from [0, 1] to [0, MTB_MaxValue_u08]
mtb_u08 constexpr mtb_Normalized_f32_To_u08(mtb_f32 Value) { return (mtb_u08)mtb_Clamp((Value * MTB_MaxValue_u08) + 0.5f, 0.0f, (mtb_f32)MTB_MaxValue_u08); }
mtb_u16 constexpr mtb_Normalized_f32_To_u16(mtb_f32 Value) { return (mtb_u16)mtb_Clamp((Value * MTB_MaxValue_u16) + 0.5f, 0.0f, (mtb_f32)MTB_MaxValue_u16); }
mtb_u32 constexpr mtb_Normalized_f32_To_u32(mtb_f32 Value) { return (mtb_u32)mtb_Clamp((Value * MTB_MaxValue_u32) + 0.5f, 0.0f, (mtb_f32)MTB_MaxValue_u32); }
mtb_u64 constexpr mtb_Normalized_f32_To_u64(mtb_f32 Value) { return (mtb_u64)mtb_Clamp((Value * MTB_MaxValue_u64) + 0.5f, 0.0f, (mtb_f32)MTB_MaxValue_u64); }

mtb_f32 constexpr mtb_Normalized_u08_To_f32(mtb_u08 Value) { return mtb_Clamp((mtb_f32)Value / MTB_MaxValue_u08, 0.0f, 1.0f); }
mtb_f32 constexpr mtb_Normalized_u16_To_f32(mtb_u16 Value) { return mtb_Clamp((mtb_f32)Value / MTB_MaxValue_u16, 0.0f, 1.0f); }
mtb_f32 constexpr mtb_Normalized_u32_To_f32(mtb_u32 Value) { return mtb_Clamp((mtb_f32)Value / MTB_MaxValue_u32, 0.0f, 1.0f); }
mtb_f32 constexpr mtb_Normalized_u64_To_f32(mtb_u64 Value) { return mtb_Clamp((mtb_f32)Value / MTB_MaxValue_u64, 0.0f, 1.0f); }


struct mtb_impl_defer
{
  template<typename LambdaType>
  struct defer
  {
    LambdaType Lambda;
    defer(LambdaType InLambda) : Lambda{ mtb_Move(InLambda) } {}
    ~defer() { Lambda(); }
  };

  template<typename FuncT>
  defer<FuncT> operator =(FuncT InLambda) { return { mtb_Move(InLambda) }; }
};

/// Defers execution of code until the end of the current scope.
///
/// Usage:
///   int i = 0;
///   MTB_DEFER [&](){ i++; printf("Foo %d\n", i); };
///   MTB_DEFER [&](){ i++; printf("Bar %d\n", i); };
///   MTB_DEFER [=](){      printf("Baz %d\n", i); };
///
/// Output:
///   Baz 0
///   Bar 1
///   Foo 2
///
/// \param CaptureSpec The lambda capture specification.
#define MTB_DEFER auto MTB_CONCAT(_Defer, MTB_CURRENT_LINE) = mtb_impl_defer() =

#endif // !defined(MTB_HEADER_mtb_common)


//
// Implementation
//

#if defined(MTB_COMMON_IMPLEMENTATION)

// Impl guard.
#if !defined(MTB_IMPL_mtb_common)
#define MTB_IMPL_mtb_common

#include <math.h>


mtb_concat_strings_result
mtb_ConcatStrings(size_t HeadSize, char const* HeadPtr, size_t TailSize, char const* TailPtr, size_t BufferSize, char* BufferPtr)
{
  size_t BufferIndex{};
  size_t HeadIndex{};
  size_t TailIndex{};

  while(BufferIndex < BufferSize && HeadIndex < HeadSize)
  {
    BufferPtr[BufferIndex++] = HeadPtr[HeadIndex++];
  }

  while(BufferIndex < BufferSize && TailIndex < TailSize)
  {
    BufferPtr[BufferIndex++] = TailPtr[TailIndex++];
  }

  if(BufferIndex < BufferSize)
  {
    // Append the trailing zero-terminator without increasing the BufferIndex.
    BufferPtr[BufferIndex] = '\0';
  }

  mtb_concat_strings_result Result{ BufferIndex, BufferPtr };
  return Result;
}

mtb_f32
mtb_Pow(mtb_f32 Base, mtb_f32 Exponent)
{
  return powf(Base, Exponent);
}

mtb_f64
mtb_Pow(mtb_f64 Base, mtb_f64 Exponent)
{
  return pow(Base, Exponent);
}

mtb_f32
mtb_Mod(mtb_f32 Value, mtb_f32 Divisor)
{
  return fmodf(Value, Divisor);
}

mtb_f64
mtb_Mod(mtb_f64 Value, mtb_f64 Divisor)
{
  return fmod(Value, Divisor);
}

mtb_f32
mtb_Sqrt(mtb_f32 Value)
{
  return sqrtf(Value);
}

mtb_f64
mtb_Sqrt(mtb_f64 Value)
{
  return sqrt(Value);
}

mtb_f32
mtb_InvSqrt(mtb_f32 Value)
{
  union FloatInt
  {
    mtb_f32 Float;
    int Int;
  };
  FloatInt MagicNumber;
  mtb_f32 HalfValue;
  mtb_f32 Result;
  const mtb_f32 ThreeHalfs = 1.5f;

  HalfValue = Value * 0.5f;
  Result = Value;
  MagicNumber.Float = Result;                               // evil floating point bit level hacking
  MagicNumber.Int  = 0x5f3759df - ( MagicNumber.Int >> 1 ); // what the fuck?
  Result = MagicNumber.Float;
  Result = Result * ( ThreeHalfs - ( HalfValue * Result * Result ) ); // 1st iteration

  return Result;
}

mtb_f32
mtb_RoundDown(mtb_f32 Value)
{
  return floorf(Value);
}

mtb_f64
mtb_RoundDown(mtb_f64 Value)
{
  return floor(Value);
}

mtb_f32
mtb_RoundUp(mtb_f32 Value)
{
  return ceilf(Value);
}

mtb_f64
mtb_RoundUp(mtb_f64 Value)
{
  return ceil(Value);
}


bool
mtb_AreNearlyEqual(mtb_f64 A, mtb_f64 B, mtb_f64 Epsilon)
{
  return mtb_Abs(A - B) <= Epsilon;
}

bool
mtb_AreNearlyEqual(mtb_f32 A, mtb_f32 B, mtb_f32 Epsilon)
{
  return mtb_Abs(A - B) <= Epsilon;
}

#endif // !defined(MTB_IMPL_mtb_common)
#endif // defined(MTB_COMMON_IMPLEMENTATION)
