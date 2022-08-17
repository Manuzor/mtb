/**

Basic usage
===========
```
// Most of your .cpp files should include it like this:
#include "mtb.h"

// But in exactly ONE of your .cpp files you additionally need to define MTB_IMPLEMENTATION before you include it.
#define MTB_IMPLEMENTATION
#include "mtb.h"
```

There are some defines (like MTB_USE_LIBC) that you can configure before
including this file. You can find all options by searching for "#Option" in
this file.

Natvis support
==============
Take the contents of the comment at the end of this file and
save it in a `mtb.natvis` file somewhere reachable by your debugger.

*/

#if !defined(MTB__INCLUDED)
#define MTB__INCLUDED

//
// Enable usually disabled code for browsing with Visual Studio's Intellisense.
//
#if defined(__INTELLISENSE__)

#undef MTB_IMPLEMENTATION
#define MTB_IMPLEMENTATION

#undef MTB_USE_STB_SPRINTF
#include "stb_sprintf.h"

#endif

//
// Check compiler
//
#define MTB_COMPILER_MSVC 0
#define MTB_COMPILER_CLANG 0

#if defined(_MSC_VER)
#undef MTB_COMPILER_MSVC
#define MTB_COMPILER_MSVC 1
#endif

#if defined(__clang__)
#undef MTB_COMPILER_CLANG
#define MTB_COMPILER_CLANG 1
#endif

//
// Check platform
//
#define MTB_PLATFORM_WIN32 0

#if defined(_WIN32)
#undef MTB_PLATFORM_WIN32
#define MTB_PLATFORM_WIN32 1
#endif

// #Option
#if !defined(MTB_USE_LIBC)
#define MTB_USE_LIBC 1
#endif

// #Option
#if !defined(MTB_TESTS)
// Auto-detect whether tests are included or not.
#if defined(DOCTEST_LIBRARY_INCLUDED)
#define MTB_TESTS 1
#else
#define MTB_TESTS 0
#endif
#endif

// #Option
#if !defined(MTB_USE_STB_SPRINTF)
// Auto-detect whether stb_printf is available
#if defined(STB_SPRINTF_H_INCLUDE)
#define MTB_USE_STB_SPRINTF 1
#else
#define MTB_USE_STB_SPRINTF 0
#endif
#endif

#include <float.h>  // FLT_MAX, DBL_MAX, LDBL_MAX
#include <new>      // Placement-new
#include <stdarg.h> // va_list, va_start, va_end
#include <stdint.h> // uint8_t, uint16_t, ..., uintptr_t

#if MTB_USE_LIBC

#include <assert.h> // assert
#include <stdlib.h> // realloc
#include <string.h> // memcpy, memmove

#endif // MTB_USE_LIBC

namespace mtb
{
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using s8 = int8_t;
    using s16 = int16_t;
    using s32 = int32_t;
    using s64 = int64_t;

    using usize = size_t;
    using ssize = ptrdiff_t;
    using uptr = uintptr_t;

    using f32 = float;
    using f64 = double;

} // namespace mtb

#define MTB_UNUSED(...) (void)(__VA_ARGS__)

// #Option
#if !defined(MTB_inline)
#define MTB_inline inline
#endif

// #Option
#if !defined(MTB_memcpy)
#define MTB_memcpy ::mtb::impl::CopyRaw
#endif

// #Option
#if !defined(MTB_memmove)
#define MTB_memmove ::mtb::impl::MoveRaw
#endif

// #Option
#if !defined(MTB_memset)
#define MTB_memset ::mtb::impl::SetRaw
#endif

// #Option
#if !defined(MTB_memcmp)
#define MTB_memcmp ::mtb::impl::CompareRaw
#endif

namespace mtb::impl
{
    void CopyRaw(void* Destination, void const* Source, usize Size);
    void MoveRaw(void* Destination, void const* Source, usize Size);
    void SetRaw(void* Destination, int ByteValue, usize Size);
    int CompareRaw(void const* A, void const* B, usize Size);
} // namespace mtb::impl

// #Option
// #TODO Create better assertion macro that accepts a format text.
#if !defined(MTB_ASSERT)
#if MTB_TESTS
#define MTB_ASSERT(...)                 \
    do                                  \
    {                                   \
        if(!(__VA_ARGS__))              \
        {                               \
            DOCTEST_FAIL(#__VA_ARGS__); \
        }                               \
    } while(false)
#elif MTB_USE_LIBC
#define MTB_ASSERT assert
#else
#define MTB_ASSERT(...)                        \
    do                                         \
    {                                          \
        if (!(__VA_ARGS__))                    \
        {                                      \
            /* Do something obviously stupid*/ \
            *(int*)0 = 42;                     \
        }                                      \
    } while (false)
#endif
#endif

#define MTB_CONCAT(A, B) MTB__CONCAT0(A, B)
#define MTB__CONCAT0(A, B) MTB__CONCAT1(A, B)
#define MTB__CONCAT1(A, B) A##B

#define MTB_SIZEOF(TYPE) (::mtb::tSizeOf<TYPE>::Value)
namespace mtb
{
    // clang-format off
    template<typename T> struct tSizeOf                      { static const usize Value = sizeof(T); };
    template<>           struct tSizeOf<void>                { static const usize Value = 1; };
    template<>           struct tSizeOf<void const>          { static const usize Value = 1; };
    template<>           struct tSizeOf<void volatile>       { static const usize Value = 1; };
    template<>           struct tSizeOf<void volatile const> { static const usize Value = 1; };
    // clang-format on
} // namespace mtb

#define MTB_ALIGNOF(TYPE) (::mtb::tAlignOf<TYPE>::Value)
namespace mtb
{
    // clang-format off
    template<typename T> struct tAlignOf                      { static const usize Value = alignof(T); };
    template<>           struct tAlignOf<void>                { static const usize Value = 1; };
    template<>           struct tAlignOf<void const>          { static const usize Value = 1; };
    template<>           struct tAlignOf<void volatile>       { static const usize Value = 1; };
    template<>           struct tAlignOf<void volatile const> { static const usize Value = 1; };
    // clang-format on
} // namespace mtb

/// Number of elements in a static array. Can be used in a static context.
///
/// You should prefer mtb::ArrayCount if possible since that one is type-safe.
///
/// Example:
///   struct tFoo {
///       u32 Numbers[128];
///       u8 Bytes[MTB_ARRAY_COUNT(Numbers)]; // sizeof(Bytes) == 128
///   };
#define MTB_ARRAY_COUNT(ARRAY) (sizeof(ARRAY) / sizeof((ARRAY)[0]))
namespace mtb
{
    /// Number of elements in a static array.
    ///
    /// Example:
    ///   u32 Stuff[128];
    ///   for(ssize Index = 0; Index < ArrayCount(Stuff); ++Index) {
    ///       printf("Stack@0x%x: %u \n", Stuff + Index, Stuff[Index]);
    ///   }
    template<typename T, ssize N>
    MTB_inline constexpr ssize ArrayCount(T (&Array)[N])
    {
        return N;
    }

    /// The same result as sizeof(Array) but type-safe (only accepts static arrays).
    template<typename T, usize N>
    MTB_inline constexpr usize ArraySize(T (&Array)[N])
    {
        return N * MTB_SIZEOF(T);
    }

    template<typename T>
    MTB_inline constexpr T* PtrOffset(T* Ptr, ssize Offset)
    {
        return (T*)(((uptr)Ptr) + (Offset * MTB_SIZEOF(T)));
    }

    template<typename T>
    MTB_inline constexpr ssize PtrDistance(T* A, T* B)
    {
        return ((ssize)(uptr)A - (ssize)(uptr)B) / MTB_SIZEOF(T);
    }
} // namespace mtb

//
// #Section Defer
//
#define MTB_DEFER auto MTB_CONCAT(_defer, __LINE__) = ::mtb::impl::DeferHelper() = [&]()
namespace mtb::impl
{
    struct DeferHelper
    {
        template<typename Defer_Func>
        struct Impl
        {
            Defer_Func& deferred;

            Impl() = delete;

            MTB_inline Impl(Defer_Func&& in_deferred)
            : deferred{in_deferred} {}

            MTB_inline ~Impl() { deferred(); }
        };

        template<typename Defer_Func>
        MTB_inline Impl<Defer_Func> operator=(Defer_Func&& in_deferred)
        {
            return Impl<Defer_Func>{(Defer_Func &&) in_deferred};
        }
    };
} // namespace mtb::impl

//
// #Section POD - plain old data
//
#if MTB_COMPILER_MSVC
#define MTB_IS_POD_STRICT(...) (__is_pod(__VA_ARGS__))
#else
#include <type_traits>
#define MTB_IS_POD_STRICT(...) (::std::is_pod<__VA_ARGS__>::value)
#endif

#define MTB_IS_POD(...) (::mtb::tIsPOD<__VA_ARGS__>::Value)
namespace mtb
{
    // clang-format off
    template<typename T> struct tIsPOD                      { static const bool Value = MTB_IS_POD_STRICT(T); };
    template<>           struct tIsPOD<void>                { static const bool Value = true; };
    template<>           struct tIsPOD<void const>          { static const bool Value = true; };
    template<>           struct tIsPOD<void volatile>       { static const bool Value = true; };
    template<>           struct tIsPOD<void const volatile> { static const bool Value = true; };
    // clang-format on
} // namespace mtb

namespace mtb
{
    namespace impl
    {
        // clang-format off
        template<typename T> struct tRemoveReference      { using tType = T; };
        template<typename T> struct tRemoveReference<T&>  { using tType = T; };
        template<typename T> struct tRemoveReference<T&&> { using tType = T; };
        // clang-format on
    } // namespace impl

    // clang-format off
    template<typename T> using tRemoveReference = typename impl::tRemoveReference<T>::tType;
    // clang-format on

    namespace impl
    {
        // clang-format off
        template<typename T> struct tRemoveConst                   { using tType = T; };
        template<typename T> struct tRemoveConst<T const>          { using tType = T; };
        template<typename T> struct tRemoveConst<T const volatile> { using tType = T volatile; };
        // clang-format on
    } // namespace impl

    // clang-format off
    template<typename T> using tRemoveConst = typename impl::tRemoveConst<T>::tType;
    // clang-format on

    namespace impl
    {
        // clang-format on
        template<typename T> struct tDecay                    { using tType = T; };
        template<typename T> struct tDecay<T&>                { using tType = T; };
        template<typename T> struct tDecay<T const>           { using tType = T; };
        template<typename T> struct tDecay<T const&>          { using tType = T; };
        template<typename T> struct tDecay<T volatile>        { using tType = T; };
        template<typename T> struct tDecay<T volatile&>       { using tType = T; };
        template<typename T> struct tDecay<T volatile const>  { using tType = T; };
        template<typename T> struct tDecay<T volatile const&> { using tType = T; };
        // clang-format off
    }
    template<typename T> using tDecay = typename impl::tDecay<T>::tType;

} // namespace mtb

namespace mtb
{
    // clang-format off
    template<typename T> constexpr T&& ForwardCast(tRemoveReference<T>&  Value) { return static_cast<T&&>(Value); }
    template<typename T> constexpr T&& ForwardCast(tRemoveReference<T>&& Value) { return static_cast<T&&>(Value); }
    template<typename T> constexpr tRemoveReference<T>&& MoveCast(T&& Value) { return static_cast<tRemoveReference<T>&&>(Value); }
    // clang-format on

    template<typename T>
    void Swap(T& A, T& B)
    {
        T Temp{MoveCast(A)};
        A = MoveCast(B);
        B = MoveCast(Temp);
    }
} // namespace mtb

//
// #Section Type properties
//
namespace mtb
{
    template<typename T>
    struct tIntProperties
    {
        static constexpr bool IsSigned = ((T)-1) < 0;
        static constexpr auto NumBits = sizeof(T) * 8;
        static constexpr T MinValue = IsSigned ? (T)((u64)1 << (NumBits - 1)) : 0;
        static constexpr T MaxValue = (T)(~MinValue);
    };

    template<typename Out, typename In>
    MTB_inline Out IntCast(In Value)
    {
        Out Result = (Out)Value;
        MTB_ASSERT((In)Result == Value && "Cast would truncate.");
        return Result;
    }

    template<typename T>
    struct tFloatProperties;

    template<>
    struct tFloatProperties<float>
    {
        static constexpr float MaxValue = FLT_MAX;
        static constexpr float MinValue = -FLT_MAX;

        static constexpr float SmallNumber = 0.0001f;
    };

    template<>
    struct tFloatProperties<double>
    {
        static constexpr double MaxValue = DBL_MAX;
        static constexpr double MinValue = -DBL_MAX;
    };

    template<>
    struct tFloatProperties<long double>
    {
        static constexpr long double MaxValue = LDBL_MAX;
        static constexpr long double MinValue = -LDBL_MAX;
    };
} // namespace mtb

//
// #Section Item Construction
//
namespace mtb
{
    // Optimized ops for POD types
    template<bool Pod = true>
    struct tItemOps
    {
        template<typename T>
        static void DefaultConstruct(T* Items, usize ItemCount)
        {
            MTB_memset(Items, 0, ItemCount * MTB_SIZEOF(T));
        }

        template<typename T, typename U>
        static void CopyConstruct(T* Destination, usize DestinationCount, U* Source, usize SourceCount)
        {
            static_assert(MTB_SIZEOF(T) == MTB_SIZEOF(U), "POD types must have the same size.");
            MTB_ASSERT((MTB_SIZEOF(T) * DestinationCount) >= (MTB_SIZEOF(U) * SourceCount));
            MTB_memmove(Destination, Source, SourceCount * MTB_SIZEOF(U));
        }

        template<typename T, typename U>
        MTB_inline static void CopyAssign(T* Destination, usize DestinationCount, U* Source, usize SourceCount)
        {
            CopyConstruct(Destination, DestinationCount, Source, SourceCount);
        }

        MTB_inline static void SetConstruct(void* Items, usize ItemCount, int RepeatedItem) { MTB_memset(Items, RepeatedItem, ItemCount); }
        MTB_inline static void SetConstruct(u8* Items, usize ItemCount, int RepeatedItem) { MTB_memset(Items, RepeatedItem, ItemCount); }
        MTB_inline static void SetConstruct(s8* Items, usize ItemCount, int RepeatedItem) { MTB_memset(Items, RepeatedItem, ItemCount); }

        template<typename T, typename U>
        static void SetConstruct(T* Items, usize ItemCount, U RepeatedItem)
        {
            for(usize Index = 0; Index < ItemCount; ++Index)
            {
                new(Items + Index) T(RepeatedItem);
            }
        }

        template<typename T, typename U>
        static void SetAssign(T* Items, usize ItemCount, U RepeatedItem)
        {
            for(usize Index = 0; Index < ItemCount; ++Index)
            {
                Items[Index] = RepeatedItem;
            }
        }

        template<typename T>
        MTB_inline static void Destruct(T* Items, usize ItemCount)
        {
            // Nothing to do for POD types.
        }

        template<typename T, typename U>
        static void Relocate(T* Destination, usize DestinationCount, U* Source, usize SourceCount)
        {
            static_assert(MTB_SIZEOF(T) == MTB_SIZEOF(U), "POD types must have the same size.");
            MTB_ASSERT((MTB_SIZEOF(T) * DestinationCount) >= (MTB_SIZEOF(U) * SourceCount));
            MTB_memmove(Destination, Source, SourceCount * MTB_SIZEOF(U));
        }
    };

    // Slow ops for non-POD types
    template<>
    struct tItemOps<false>
    {
        template<typename T>
        static void DefaultConstruct(T* Items, usize ItemCount)
        {
            for (usize Index = 0; Index < ItemCount; ++Index)
            {
                new (Items + Index) T();
            }
        }

        template<typename T, typename U>
        static void CopyConstruct(T* Destination, usize DestinationCount, U* Source, usize SourceCount)
        {
            MTB_ASSERT(DestinationCount >= SourceCount);
            for(usize Index = 0; Index < SourceCount; ++Index)
            {
                new(Destination + Index) T(Source[Index]);
            }
        }

        template<typename T, typename U>
        static void CopyAssign(T* Destination, usize DestinationCount, U* Source, usize SourceCount)
        {
            MTB_ASSERT(DestinationCount >= SourceCount);
            for(usize Index = 0; Index < SourceCount; ++Index)
            {
                Destination[Index] = Source[Index];
            }
        }

        template<typename T, typename U>
        static void SetConstruct(T* Items, usize ItemCount, U RepeatedItem)
        {
            for(usize Index = 0; Index < ItemCount; ++Index)
            {
                new(Items + Index) T(RepeatedItem);
            }
        }

        template<typename T, typename U>
        static void SetAssign(T* Items, usize ItemCount, U RepeatedItem)
        {
            for(usize Index = 0; Index < ItemCount; ++Index)
            {
                Items[Index] = RepeatedItem;
            }
        }

        template<typename T>
        static void Destruct(T* Items, usize ItemCount)
        {
            for (usize Index = 0; Index < ItemCount; ++Index)
            {
                Items[Index].~T();
            }
        }

        template<typename T, typename U>
        static void Relocate(T* Destination, U* Source, usize SourceCount)
        {
            for (usize Index = 0; Index < SourceCount; ++Index)
            {
                new (Destination + Index) T(Source[Index]); // #TODO Do we need MoveCast(Source[Index]) here?
                Source[Index].~U();
            }
        }
    };
} // namespace mtb

//
// #Section Slice
//
namespace mtb
{
    // #Note Slice specialization could make use of inheritance.
    //       However, some compilers no longer recognize such slices as POD types.
    //

    // #Note Due to conversion operators from mutable to const that we implement, the const versions of tSlice
    //       have to be declared before the mutable version. In other words, declaration order is important here.
    //       Don't rearrange.
    template<typename T>
    struct tSlice;

    // special case - void const
    template<>
    struct tSlice<void const>
    {
        void const* Ptr;
        ssize Count;

        // C++ 11 range-based for interface
        constexpr void const* begin() const { return Ptr; }
        // C++ 11 range-based for interface
        constexpr void const* end() const { return PtrOffset(Ptr, Count); }

        // C-Array like pointer offset. Slice + 3 is equivalent to Slice.Ptr + 3, except the former does bounds checking.
        MTB_inline void const* operator+(ssize Index) const
        {
            MTB_ASSERT(0 <= Index && Index < Count);
            void const* Result = PtrOffset(Ptr, Index);
            return Result;
        }

        MTB_inline constexpr explicit operator bool() const { return Ptr && Count > 0; }
    };

    // special case - void (mutable)
    template<>
    struct tSlice<void>
    {
        void* Ptr;
        ssize Count;

        // C++ 11 range-based for interface
        constexpr void* begin() const { return Ptr; }
        // C++ 11 range-based for interface
        constexpr void* end() const { return PtrOffset(Ptr, Count); }

        // C-Array like pointer offset. Slice + 3 is equivalent to Slice.Ptr + 3, except the former does bounds checking.
        MTB_inline void* operator+(ssize Index) const
        {
            MTB_ASSERT(0 <= Index && Index < Count);
            void* Result = PtrOffset(Ptr, Index);
            return Result;
        }

        MTB_inline constexpr explicit operator bool() const { return Ptr && Count > 0; }

        // Implicit conversion to the const version.
        MTB_inline constexpr operator tSlice<void const>() const { return {Ptr, Count}; }
    };

    // special case - T const
    template<typename T>
    struct tSlice<T const>
    {
        T const* Ptr;
        ssize Count;

        // C++ 11 range-based for interface
        constexpr T const* begin() const { return Ptr; }
        // C++ 11 range-based for interface
        constexpr T const* end() const { return PtrOffset(Ptr, Count); }

        // C-Array like pointer offset. Slice + 3 is equivalent to Slice.Ptr + 3, except the former does bounds checking.
        MTB_inline T const* operator+(ssize Index) const
        {
            MTB_ASSERT(0 <= Index && Index < Count);
            T const* Result = PtrOffset(Ptr, Index);
            return Result;
        }

        MTB_inline constexpr explicit operator bool() const { return Ptr && Count > 0; }

        MTB_inline T const& operator[](ssize Index) const { return *(*this + Index); }

        MTB_inline constexpr operator tSlice<void const>() const { return {Ptr, (ssize)(sizeof(T) * Count)}; }
    };

    // general case - T (mutable)
    template<typename T>
    struct tSlice
    {
        T* Ptr;
        ssize Count;

        // C++ 11 range-based for interface
        constexpr T* begin() const { return Ptr; }
        // C++ 11 range-based for interface
        constexpr T* end() const { return PtrOffset(Ptr, Count); }

        // C-Array like pointer offset. Slice + 3 is equivalent to Slice.Ptr + 3, except the former does bounds checking.
        MTB_inline T* operator+(ssize Index) const
        {
            MTB_ASSERT(0 <= Index && Index < Count);
            T* Result = PtrOffset(Ptr, Index);
            return Result;
        }

        MTB_inline constexpr explicit operator bool() const { return Ptr && Count > 0; }

        MTB_inline T& operator[](ssize Index) const { return *(*this + Index); }

        // Implicit conversion to the const version.
        constexpr operator tSlice<T const>() const { return {Ptr, Count}; }
        constexpr operator tSlice<void>() const { return {Ptr, (ssize)(sizeof(T) * Count)}; }
        constexpr operator tSlice<void const>() const { return {Ptr, (ssize)(sizeof(T) * Count)}; }
    };

    static_assert(MTB_IS_POD_STRICT(tSlice<char>), "char slices must be PODs");
    static_assert(MTB_IS_POD_STRICT(tSlice<char const>), "char slices must be PODs");
    static_assert(MTB_IS_POD_STRICT(tSlice<void>), "void slices must be PODs");
} // namespace mtb

namespace mtb
{
    template<typename T>
    MTB_inline constexpr bool IsValidIndex(tSlice<T> Slice, ssize Index)
    {
        return 0 <= Index && Index < Slice.Count;
    }

    template<typename T>
    MTB_inline void CheckIndex(tSlice<T> Slice, ssize Index)
    {
        MTB_ASSERT(0 <= Index && Index < Slice.Count);
    }

    template<typename T>
    MTB_inline constexpr usize SliceSize(tSlice<T> Slice)
    {
        return MTB_SIZEOF(T) * Slice.Count;
    }

    template<typename T>
    MTB_inline constexpr T* SliceLast(tSlice<T> Slice)
    {
        return Slice.Count == 0 ? nullptr : PtrOffset(Slice.Ptr, Slice.Count - 1);
    }

    template<typename T>
    MTB_inline constexpr tSlice<T> PtrSlice(T* Ptr, ssize Count)
    {
        MTB_ASSERT(Ptr || !Count);
        return {Ptr, Count};
    }

    template<typename T>
    MTB_inline constexpr tSlice<T> PtrSliceBetween(T* StartPtr, T* EndPtr)
    {
        return {StartPtr, IntCast<ssize>(((uptr)EndPtr - (uptr)StartPtr) / MTB_SIZEOF(T))};
    }

    template<typename T, usize N>
    MTB_inline constexpr tSlice<T> ArraySlice(T (&Array)[N])
    {
        return {&Array[0], N};
    }

    template<typename T>
    tSlice<T> SliceRemoveConst(tSlice<T const> Slice) { return PtrSlice(const_cast<T*>(Slice.Ptr), Slice.Count); }

    template<typename T>
    tSlice<T> SliceRange(tSlice<T> Slice, ssize Offset, ssize Count)
    {
        // #TODO Should we just do a boundscheck here?
#if 1
        MTB_ASSERT(0 <= Count && Count <= Slice.Count + Offset);
#else
        if(Count < 0)
        {
            Count = 0;
        }
        if(Count > Slice.Count + Offset)
        {
            Count = Slice.Count - Offset;
        }
#endif

        return PtrSlice(PtrOffset(Slice.Ptr, Offset), Count);
    }

    template<typename T>
    MTB_inline tSlice<T> SliceBetween(tSlice<T> Slice, ssize FirstIndex, ssize OnePastLastIndex)
    {
        return SliceRange(Slice, FirstIndex, OnePastLastIndex - FirstIndex);
    }

    template<typename T>
    MTB_inline tSlice<T> SliceOffset(tSlice<T> Slice, ssize Offset)
    {
        return SliceBetween(Slice, Offset, Slice.Count);
    }

    template<typename T, usize N>
    MTB_inline constexpr tSlice<T> ArraySliceRange(T (&Array)[N], ssize Offset, ssize Count)
    {
        return SliceRange(ArraySlice(Array), Offset, Count);
    }

    template<typename T, usize N>
    MTB_inline constexpr tSlice<T> ArraySliceBetween(T (&Array)[N], ssize FirstIndex, ssize OnePastLastIndex)
    {
        return SliceBetween(ArraySlice(Array), FirstIndex, OnePastLastIndex);
    }

    template<typename T, usize N>
    MTB_inline constexpr tSlice<T> ArraySliceOffset(T (&Array)[N], ssize Offset)
    {
        return SliceBetween(ArraySlice(Array), Offset, N);
    }

    template<typename U, typename T>
    MTB_inline constexpr tSlice<U> SliceCast(tSlice<T> Slice)
    {
        static_assert(MTB_SIZEOF(U) < MTB_SIZEOF(T) || MTB_SIZEOF(U) % MTB_SIZEOF(T) == 0, "Unable to reinterpret.");
        return PtrSliceBetween(reinterpret_cast<U*>(Slice.begin()), reinterpret_cast<U*>(Slice.end()));
    }

    template<typename D, typename S>
    void SliceCopyRaw(tSlice<D> Destination, tSlice<S> Source)
    {
        MTB_ASSERT(SliceSize(Destination) >= SliceSize(Source));
        MTB_memcpy(Destination.Ptr, Source.Ptr, SliceSize(Source));
    }

    template<typename T>
    bool SliceIsZero(tSlice<T> Slice)
    {
        //tSlice<u8 const> Bytes = SliceCast<u8 const>(Slice);
        tSlice<u8 const> Bytes = PtrSlice((u8 const*)Slice.Ptr, SliceSize(Slice));
        bool bResult = true;
        for(u8 Byte : Bytes)
        {
            if(Byte)
            {
                bResult = false;
                break;
            }
        }
        return bResult;
    }

    template<typename T>
    MTB_inline void SliceDefaultConstructItems(tSlice<T> Slice)
    {
        tItemOps<MTB_IS_POD(T)>::DefaultConstruct(Slice.Ptr, Slice.Count);
    }

    template<typename T>
    MTB_inline void SliceDestructItems(tSlice<T> Slice)
    {
        tItemOps<MTB_IS_POD(T)>::Destruct(Slice.Ptr, Slice.Count);
    }

    /// Think memset for tSlice.
    template<typename T, typename U>
    MTB_inline void SliceAssignItem(tSlice<T> Slice, U Item)
    {
        for(ssize Index = 0; Index < Slice.Count; ++Index)
        {
        }
        for(T& Dest : Slice)
        {
            Dest = Item;
        }
    }

    template<typename T, typename U>
    MTB_inline void SliceCopyConstructItems(tSlice<T> Destination, tSlice<U> Source)
    {
        MTB_ASSERT(SliceSize(Destination) >= SliceSize(Source));
        tItemOps<MTB_IS_POD(T) && MTB_SIZEOF(T) == MTB_SIZEOF(U)>::CopyConstruct(Destination.Ptr, Destination.Count, Source.Ptr, Source.Count);
    }

    template<typename T, typename U>
    MTB_inline void SliceCopyAssignItems(tSlice<T> Destination, tSlice<U> Source)
    {
        MTB_ASSERT(SliceSize(Destination) >= SliceSize(Source));
        tItemOps<MTB_IS_POD(T) && MTB_SIZEOF(T) == MTB_SIZEOF(U)>::CopyAssign(Destination.Ptr, Destination.Count, Source.Ptr, Source.Count);
    }

    template<typename T, typename U>
    MTB_inline void SliceSetConstructItems(tSlice<T> Slice, U RepeatedItem)
    {
        tItemOps<MTB_IS_POD(T)>::SetConstruct(Slice.Ptr, Slice.Count, RepeatedItem);
    }

    template<typename T, typename U>
    MTB_inline void SliceSetAssignItems(tSlice<T> Slice, U RepeatedItem)
    {
        tItemOps<MTB_IS_POD(T)>::SetAssign(Slice.Ptr, Slice.Count, RepeatedItem);
    }

    template<typename T>
    MTB_inline void SliceSetZero(tSlice<T> Slice)
    {
        tItemOps<true>::DefaultConstruct((void*)Slice.Ptr, MTB_SIZEOF(T) * Slice.Count);
    }

    // aka 'destructive move'
    template<typename T, typename U>
    MTB_inline void SliceRelocateItems(tSlice<T> Destination, tSlice<U> Source)
    {
        MTB_ASSERT(SliceSize(Destination) >= SliceSize(Source));
        tItemOps<MTB_IS_POD(T)>::Relocate(Destination.Ptr, Destination.Count, Source.Ptr, Source.Count);
    }

    template<typename T, typename U>
    bool SliceEquals(tSlice<T> A, tSlice<U> B)
    {
        bool bResult = false;
        if(A.Count == B.Count)
        {
            bResult = true;
            for(ssize Index = 0; Index < A.Count; ++Index)
            {
                if(!(A.Ptr[Index] == B.Ptr[Index]))
                {
                    bResult = false;
                    break;
                }
            }
        }
        return bResult;
    }

    bool SliceEqualsRaw(tSlice<void const> A, tSlice<void const> B);

    int SliceCompareRaw(tSlice<void const> A, tSlice<void const> B);

    template<typename T, typename U>
    bool SliceStartsWith(tSlice<T> A, tSlice<U> B)
    {
        bool bResult = false;
        if(A.Count >= B.Count)
        {
            bResult = SliceEquals(SliceBetween(A, 0, B.Count), B);
        }
        return bResult;
    }

    template<typename T, typename U>
    bool SliceEndsWith(tSlice<T> A, tSlice<U> B)
    {
        bool bResult = false;
        if(A.Count >= B.Count)
        {
            bResult = SliceEquals(SliceOffset(A, A.Count - B.Count), B);
        }
        return bResult;
    }

    template<typename T, typename P>
    tSlice<T> SliceTrimStartByPredicate(tSlice<T> Slice, P Predicate)
    {
        ssize Offset = 0;
        for(T const& Item : Slice)
        {
            if(!Predicate(Item))
            {
                break;
            }
            ++Offset;
        }
        return SliceOffset(Slice, Offset);
    }

    template<typename T, typename P>
    tSlice<T> SliceTrimEndByPredicate(tSlice<T> Slice, P Predicate)
    {
        ssize Count = Slice.Count;
        for(; Count > 0; --Count)
        {
            if(!Predicate(Slice.Ptr[Count - 1]))
            {
                break;
            }
        }
        return SliceBetween(Slice, 0, Count);
    }

    template<typename T, typename P>
    tSlice<T> SliceTrimByPredicate(tSlice<T> Slice, P Predicate)
    {
        return SliceTrimStartByPredicate(SliceTrimEndByPredicate(Slice, Predicate), Predicate);
    }

    template<typename T, typename U>
    T* SliceFind(tSlice<T> Haystack, U const& Needle)
    {
        T* Result = nullptr;
        for(ssize Index = 0; Index < Haystack.Count; ++Index)
        {
            T* Item = Haystack.Ptr + Index;
            if(*Item == Needle)
            {
                Result = Item;
                break;
            }
        }
        return Result;
    }

    template<typename T, typename P>
    T* SliceFindByPredicate(tSlice<T> Haystack, P Predicate)
    {
        T* Result = nullptr;
        for(ssize Index = 0; Index < Haystack.Count; ++Index)
        {
            T* Item = Haystack.Ptr + Index;
            if(Predicate(*Item))
            {
                Result = Item;
                break;
            }
        }
        return Result;
    }

    template<typename T, typename U>
    T* SliceFindLast(tSlice<T> Haystack, U const& Needle)
    {
        T* Result = nullptr;
        for(ssize rIndex = Haystack.Count; rIndex > 0;)
        {
            T* Item = Haystack.Ptr + --rIndex;
            if(*Item == Needle)
            {
                Result = Item;
                break;
            }
        }
        return Result;
    }

    template<typename T, typename P>
    T* SliceFindLastByPrediate(tSlice<T> Haystack, P Predicate)
    {
        T* Result = nullptr;
        for(ssize rIndex = Haystack.Count; rIndex > 0;)
        {
            T* Item = Haystack.Ptr + --rIndex;
            if(Predicate(*Item))
            {
                Result = Item;
                break;
            }
        }
        return Result;
    }
} // namespace mtb

//
// #Section Memory Allocation
//

#if !defined(MTB_ALLOCATOR_DEFAULT_ALIGNMENT)
#define MTB_ALLOCATOR_DEFAULT_ALIGNMENT 16
#endif

#if MTB_ALLOCATOR_DEFAULT_ALIGNMENT <= 0
#error "MTB_ALLOCATOR_DEFAULT_ALIGNMENT must be non-zero."
#endif

namespace mtb
{
    void AlignAllocation(void** inout_Ptr, usize* inout_Size, usize Alignment);

    template<typename T>
    bool StructIsZero(T const& One) { return SliceIsZero(PtrSlice(&One, 1)); }

    /// Compare the bytes of A and B. Will always return false if the byte sizes differ.
    template<typename T, typename U>
    bool StructEqualsRaw(T A, U B)
    {
        static_assert(MTB_SIZEOF(T) == MTB_SIZEOF(U), "Types must be of same size, otherwise an equality test of raw memory will not be useful.");
        return MTB_memcmp(&A, &B, MTB_SIZEOF(T)) == 0;
    }

    // Impl will reinterpret as needed.
    struct tAllocMarker
    {
        // #TODO Is that enough for everybody?
        uptr Data[2];
    };

    namespace eAlloc
    {
        enum Type
        {
            kNoInit,
            kClearToZero,
            kDefaultConstruct,
        };
    }

    // nullptr is allowed for all function pointers. It means the allocator doesn't support the operation.
    // Use functions like mtb::AllocRaw(MyAllocator, 123) instead of MyAllocator.AllocRaw(MyAllocator.User, 123);
    struct tAllocator
    {
        void* User;
        usize DefaultAlignment;

        tSlice<void> (*ReallocProc)(void* User, tSlice<void> OldMem, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero);

        MTB_inline constexpr explicit operator bool() const { return ReallocProc; }
    };

    // Raw allocation (alloc / dealloc)
    // {
    tSlice<void> ReallocRaw(tAllocator A, tSlice<void> OldMem, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero = true);
    tSlice<void> AllocRaw(tAllocator A, usize Size, usize Alignment, bool bClearToZero = true);
    void DeallocRaw(tAllocator A, tSlice<void> Ptr, usize Alignment);
    // }

    // Typed array allocation (alloc / dealloc)
    // {
    template<typename T>
    tSlice<T> ReallocArray(tAllocator A, tSlice<T> Array, usize NewCount, bool bClearToZero = true)
    {
        tSlice<void> Raw = ReallocRaw(A, Array, MTB_ALIGNOF(T), NewCount * MTB_SIZEOF(T), MTB_ALIGNOF(T), false);
        if(bClearToZero && (usize)Array.Count < NewCount)
        {
            SliceDefaultConstructItems(SliceOffset(Raw, SliceSize(Array)));
        }
        tSlice<T> Result = SliceCast<T>(Raw);
        return Result;
    }

    template<typename T>
    tSlice<T> AllocArray(tAllocator A, usize Count, bool bClearToZero = true)
    {
        tSlice<void> Raw = AllocRaw(A, Count * MTB_SIZEOF(T), MTB_ALIGNOF(T));
        if(bClearToZero)
        {
            SliceDefaultConstructItems(Raw);
        }
        return SliceCast<T>(Raw);
    }

    template<typename TIn, typename TOut = tRemoveConst<TIn>>
    tSlice<TOut> AllocArrayCopy(tAllocator A, tSlice<TIn> Array)
    {
        tSlice<TOut> Result = AllocArray<TOut>(A, Array.Count);
        SliceCopyRaw(Result, Array);
        return Result;
    }

    template<typename T>
    void DeallocArray(tAllocator A, tSlice<T> Array)
    {
        DeallocRaw(A, Array, MTB_ALIGNOF(T));
    }
    // }

    // Typed array allocation + construction (create / destroy)
    // {
    template<typename T>
    tSlice<T> ResizeArray(tAllocator A, tSlice<T> Array, usize NewCount, bool bInit = true)
    {
        tSlice<void> Raw = ReallocRaw(A, Array, MTB_ALIGNOF(T), NewCount * MTB_SIZEOF(T), MTB_ALIGNOF(T), false);
        tSlice<T> Result = SliceCast<T>(Raw);
        if(bInit && Array.Count < (ssize)NewCount)
        {
            SliceDefaultConstructItems(SliceOffset(Result, Array.Count));
        }
        return Result;
    }

    template<typename T, typename U>
    tSlice<T> AppendArray(tAllocator A, tSlice<T> Array, tSlice<U> Appendix)
    {
        tSlice<T> Result = ResizeArray(A, Array, Array.Count + Appendix.Count);
        SliceCopyConstructItems(SliceOffset(Result, Array.Count), Appendix);
        return Result;
    }

    template<typename T>
    tSlice<T> CreateArray(tAllocator A, usize Count)
    {
        tSlice<T> Result = AllocArray<T>(A, Count);
        SliceDefaultConstructItems(Result);
        return Result;
    }

    template<typename TIn, typename TOut = tRemoveConst<TIn>>
    tSlice<TOut> CreateArrayCopy(tAllocator A, tSlice<TIn> Array)
    {
        tSlice<TOut> Result = AllocArray<TOut>(A, Array.Count);
        SliceCopyConstructItems(Result, Array);
        return Result;
    }

    template<typename T>
    void DestroyArray(tAllocator A, tSlice<T> Array)
    {
        SliceDestructItems(Array);
        DeallocArray(A, Array);
    }
    // }

    // Typed object allocation (alloc / dealloc)
    // {
    template<typename T>
    T* AllocOne(tAllocator A)
    {
        return AllocArray<T>(A, 1).Ptr;
    }

    template<typename T>
    T* AllocOneCopy(tAllocator A, T const& One)
    {
        return AllocArrayCopy(A, PtrSlice(&One, 1)).Ptr;
    }

    template<typename T>
    void DeallocOne(tAllocator A, T* One)
    {
        if(One)
        {
            DeallocRaw(A, PtrSlice(One, 1), MTB_ALIGNOF(T));
        }
    }
    // }

    // Typed object allocation + construction (create / destroy)
    // {
    template<typename T>
    T* CreateOne(tAllocator A)
    {
        return CreateArray<T>(A, 1).Ptr;
    }

    template<typename T>
    T* CreateOneCopy(tAllocator A, T const& One)
    {
        return CreateArrayCopy(A, PtrSlice(&One, 1)).Ptr;
    }

    template<typename T>
    void DestroyOne(tAllocator A, T* One)
    {
        if(One)
        {
            DestroyArray(A, PtrSlice(One, 1));
        }
    }
    // }

    tAllocator GetDefaultAllocator();

#if MTB_USE_LIBC
    tAllocator GetLibcAllocator();
#endif

#if MTB_USE_STB_SPRINTF
    /// \remark Does not account for the null-terminator.
    /// \return The formatted string EXCLUDING the null-terminator.
    tSlice<char> vprintfAlloc(tAllocator A, char const* Format, va_list VArgs);

    /// \remark Does not account for the null-terminator.
    /// \return The formatted string EXCLUDING the null-terminator.
    MTB_inline tSlice<char> printfAlloc(tAllocator A, char const* Format, ...)
    {
        va_list VArgs;
        va_start(VArgs, Format);
        tSlice<char> Result = vprintfAlloc(A, Format, VArgs);
        va_end(VArgs);
        return Result;
    }

    /// \return The formatted string INCLUDING the null-terminator.
    MTB_inline tSlice<char> vprintfAllocZ(tAllocator A, char const* Format, va_list VArgs)
    {
        tSlice<char> Result = vprintfAlloc(A, Format, VArgs);
        return AppendArray(A, Result, PtrSlice("\0", 1));
    }

    /// \return The formatted string INCLUDING the null-terminator.
    MTB_inline tSlice<char> printfAllocZ(tAllocator A, char const* Format, ...)
    {
        va_list VArgs;
        va_start(VArgs, Format);
        tSlice<char> Result = vprintfAllocZ(A, Format, VArgs);
        va_end(VArgs);
        return Result;
    }
#endif // MTB_USE_STB_SPRINTF

} // namespace mtb

namespace mtb
{
    enum eInit
    {
        kInit_None,
        kInit_Zero,
        kInit_Full,
    };

    template<typename T>
    struct tArray
    {
        /// May be null. Fallback is the result of mtb::GetDefaultAllocator()
        tAllocator Allocator;

        /// May be null. Fallback employs amortized doubling.
        ssize (*CalcCapacity)(tArray<T>& Array, ssize MinCapacity);

        union
        {
            struct
            {
                /// Pointer to first element of allocated array.
                T* Ptr;
                /// Number of elements currently in use.
                ssize Count;
            };
            /// Convenient access of the slice currently in use.
            tSlice<T> Slice;
        };
        /// Number of allocated elements.
        ssize Capacity;

        T* operator+(ssize Index) { return Slice + Index; }
        T& operator[](ssize Index) { return Slice[Index]; }
        inline constexpr explicit operator bool() const { return (bool)Slice; }

        MTB_inline T* begin() const { return Slice.begin(); }
        MTB_inline T* end() const { return Slice.end(); }
    };

    template<typename T>
    bool IsValidIndex(tArray<T> Array, ssize Index)
    {
        return IsValidIndex(Array.Slice, Index);
    }

    template<typename T>
    tAllocator GetAllocator(tArray<T>& Array)
    {
        if(!Array.Allocator)
        {
            Array.Allocator = GetDefaultAllocator();
        }
        return Array.Allocator;
    }

    template<typename T>
    bool Reserve(tArray<T>& Array, ssize MinRequestedCapacity)
    {
        if(Array.Capacity >= MinRequestedCapacity)
        {
            return true;
        }

        ssize NewAllocationCount;
        if(Array.CalcCapacity)
        {
            NewAllocationCount = Array.CalcCapacity(Array, MinRequestedCapacity);
            if(NewAllocationCount < MinRequestedCapacity)
            {
                NewAllocationCount = MinRequestedCapacity;
            }
        }
        else
        {
            NewAllocationCount = Array.Capacity > 0 ? Array.Capacity : 16;
            while(NewAllocationCount < MinRequestedCapacity)
            {
                NewAllocationCount *= 2;
            }
        }
        tSlice<T> NewAllocation = ResizeArray(GetAllocator(Array), PtrSlice(Array.Ptr, Array.Capacity), NewAllocationCount, false);
        if(NewAllocation)
        {
            Array.Ptr = NewAllocation.Ptr;
            Array.Capacity = NewAllocation.Count;
        }
        return !!NewAllocation;
    }

    template<typename T>
    tSlice<T> ShrinkAllocation(tArray<T>& Array)
    {
        tSlice<T> NewAllocation = ResizeArray(Array.Allocator, PtrSlice(Array.Ptr, Array.Capacity), Array.Count, false);
        Array.Ptr = NewAllocation.Ptr;
        Array.Capacity = NewAllocation.Count;
        return Array.Slice;
    }

    template<typename T>
    void Clear(tArray<T>& Array)
    {
        Array.Count = 0;
    }

    template<typename T>
    void ClearAllocation(tArray<T>& Array)
    {
        Clear(Array);
        ShrinkAllocation(Array);
    }

    template<typename T>
    bool SetCount(tArray<T>& Array, ssize NewCount, eInit Init = kInit_Full)
    {
        if(Array.Count < NewCount)
        {
            if(Reserve(Array, NewCount))
            {
                tSlice<T> Fresh = SliceBetween(PtrSlice(Array.Ptr, Array.Capacity), Array.Count, NewCount);
                switch(Init)
                {
                    case kInit_Zero: SliceDefaultConstructItems(SliceCast<void>(Fresh)); break;
                    case kInit_Full: SliceDefaultConstructItems(Fresh); break;
                    case kInit_None: break;
                }
                Array.Count = NewCount;
            }
        }
        else
        {
            Array.Count = NewCount;
        }
        return Array.Count == NewCount;
    }

    template<typename T>
    T* GetLast(tArray<T> Array)
    {
        return Array ? (Array + (Array.Count - 1)) : nullptr;
    }

    template<typename T>
    tSlice<T> GetSlack(tArray<T> Array)
    {
        return SliceOffset(PtrSlice(Array.Ptr, Array.Capacity), Array.Count);
    }

    template<typename T>
    usize ArraySize(tArray<T> Array)
    {
        return SliceSize(Array.Slice);
    }

    // InsertN      - N fresh items
    // InsertMany   - N copied items
    // InsertOne    - 1 fresh item
    // Insert       - 1 copied item
    // InsertRepeat - N fresh items, copied from a single item

    /// Create \a InsertCount new items at the given index and return them as a slice.
    template<typename T>
    tSlice<T> InsertN(tArray<T>& Array, ssize InsertIndex, ssize InsertCount, eInit Init = kInit_Full)
    {
        MTB_ASSERT(InsertCount > 0);
        tSlice<T> Result{};
        if(Reserve(Array, Array.Count + InsertCount))
        {
            MTB_ASSERT(IsValidIndex(Array, InsertIndex) || InsertIndex == Array.Count);
            tSlice<T> Allocation = PtrSlice(Array.Ptr, Array.Capacity);
            tSlice<T> Result = SliceRange(Allocation, InsertIndex, InsertCount);
            SliceRelocateItems(SliceOffset(Allocation, InsertIndex + InsertCount), Result);
            switch(Init)
            {
                case kInit_Zero: SliceDefaultConstructItems(SliceCast<void>(Result)); break;
                case kInit_Full: SliceDefaultConstructItems(Result); break;
                case kInit_None: break;
            }
            Array.Count += InsertCount;
        }
        return Result;
    }

    /// Create enough room for Slice items to be copied to at the specified index.
    template<typename T, typename U>
    tSlice<T> InsertMany(tArray<T>& Array, ssize InsertIndex, tSlice<U> Slice)
    {
        tSlice<T> Result = InsertN(Array, InsertIndex, Slice.Count, kInit_None);
        SliceCopyConstructItems(Result, Slice);
        return Result;
    }

    /// Create a new item in the array and return a pointer to it.
    template<typename T>
    T* InsertOne(tArray<T>& Array, ssize InsertIndex, eInit Init = kInit_Full)
    {
        return InsertN(Array, InsertIndex, 1, Init).Ptr;
    }

    /// Create a new item in the array and copy \a Item there.
    template<typename T, typename TItem>
    T* Insert(tArray<T>& Array, ssize InsertIndex, TItem Item)
    {
        return new(InsertOne<T>(Array, InsertIndex, kInit_None)) T(Item);
    }

    /// Create \a RepeatCount items in the array, initializing them all to the value of \a Item.
    template<typename T, typename TItem>
    tSlice<T> InsertRepeat(tArray<T>& Array, ssize InsertIndex, TItem Item, ssize RepeatCount)
    {
        tSlice<TItem> ItemSlice = PtrSlice(&Item, 1);
        tSlice<T> Result = InsertN(Array, InsertIndex, RepeatCount, kInit_None);
        for(ssize Index = 0; Index < RepeatCount; ++Index)
        {
            SliceCopyConstructItems(SliceOffset(Result, Index), ItemSlice);
        }
        return Result;
    }

    /// Create \a PushCount new items at the end and return them as a slice.
    template<typename T>
    tSlice<T> PushN(tArray<T>& Array, ssize PushCount, eInit Init = kInit_Full)
    {
        tSlice<T> Result{};
        if(Reserve(Array, Array.Count + PushCount))
        {
            tSlice<T> Allocation = PtrSlice(Array.Ptr, Array.Capacity);
            Result = SliceRange(Allocation, Array.Count, PushCount);
            switch(Init)
            {
                case kInit_Zero: SliceDefaultConstructItems(SliceCast<void>(Result)); break;
                case kInit_Full: SliceDefaultConstructItems(Result); break;
                case kInit_None: break;
            }
            Array.Count += PushCount;
        }
        return Result;
    }

    /// Create enough room for Slice items to be copied to.
    template<typename T, typename U>
    tSlice<T> PushMany(tArray<T>& Array, tSlice<U> Slice)
    {
        tSlice<T> Result = PushN(Array, Slice.Count, kInit_None);
        SliceCopyConstructItems(Result, Slice);
        return Result;
    }

    /// Create a new item in the array and return a pointer to it.
    template<typename T>
    T* PushOne(tArray<T>& Array, eInit Init = kInit_Full)
    {
        return PushN(Array, 1, Init).Ptr;
    }

    /// Create a new item in the array and copy \a Item there.
    template<typename T, typename TItem>
    T* Push(tArray<T>& Array, TItem Item)
    {
        return new(PushOne<T>(Array, kInit_None)) T(Item);
    }

    /// Create \a RepeatCount items in the array, initializing them all to the value of \a Item.
    template<typename T, typename TItem>
    tSlice<T> PushRepeat(tArray<T>& Array, TItem Item, ssize RepeatCount)
    {
        tSlice<TItem> ItemSlice = PtrSlice(&Item, 1);
        tSlice<T> Result = PushN(Array, RepeatCount, kInit_None);
        for(ssize Index = 0; Index < RepeatCount; ++Index)
        {
            SliceCopyConstructItems(SliceOffset(Result, Index), ItemSlice);
        }
        return Result;
    }

#if MTB_USE_STB_SPRINTF
    tSlice<char> PushFormat(tArray<char>& Array, char const* Format, ...);
    tSlice<char> PushFormatV(tArray<char>& Array, char const* Format, va_list VArgs);
#endif

    template<typename T>
    void RemoveAt(tArray<T>& Array, ssize RemoveIndex, ssize RemoveCount = 1, bool bSwap = false)
    {
        MTB_ASSERT(RemoveCount > 0);
        CheckIndex(Array.Slice, RemoveIndex);
        CheckIndex(Array.Slice, RemoveIndex + RemoveCount);
        if(bSwap)
        {
            SliceRelocateItems(SliceRange(Array.Slice, RemoveIndex, RemoveCount), SliceRange(Array.Slice, Array.Count - RemoveCount, RemoveCount));
        }
        else
        {
            SliceRelocateItems(SliceOffset(Array.Slice, RemoveIndex), SliceOffset(Array.Slice, RemoveIndex + RemoveCount));
        }
        Array.Count -= RemoveCount;
    }

    template<typename T, typename P>
    void RemoveAll(tArray<T>& Array, P Predicate, bool bSwap = false)
    {
        // #TODO
    }

    template<typename T, typename P>
    void RemoveFirst(tArray<T>& Array, P Predicate, bool bSwap = false)
    {
        // #TODO
    }

    template<typename T, typename P>
    void RemoveLast(tArray<T>& Array, P Predicate, bool bSwap = false)
    {
        // #TODO
    }

    tSlice<char> ToString(tArray<char> Array, bool bNullTerminate = true);

} // namespace mtb

//
// #Section Map
//
namespace mtb
{
    template<typename K, typename V>
    struct tMap;

    struct tMapSlot
    {
        enum eState : u8
        {
            kFree,
            kOccupied,
            kDead,
        };

        eState State;
    };
    static_assert(sizeof(tMapSlot) == sizeof(u8));

    template<typename T>
    struct tMapIterator_KeyOrValue
    {
        ssize Capacity;
        tMapSlot* Slots;
        T* Items;
        ssize Index;

        bool operator!=(tMapIterator_KeyOrValue const& Other) const
        {
            return Index != Other.Index;
        }

        T& operator*() const
        {
            MTB_ASSERT(Index < Capacity);
            return Items[Index];
        }

        tMapIterator_KeyOrValue& operator++()
        {
            while (++Index < Capacity)
            {
                if (Slots[Index].State == tMapSlot::kOccupied)
                {
                    break;
                }
            }
            return *this;
        }

        tMapIterator_KeyOrValue begin()
        {
            tMapIterator_KeyOrValue Result = *this;
            Result.Index = -1;
            return ++Result;
        }

        tMapIterator_KeyOrValue end()
        {
            tMapIterator_KeyOrValue Result = *this;
            Result.Index = Capacity;
            return Result;
        }
    };

    // #TODO Return usize instead? May be easier to leave it up to the user to take care of bitness.
    using tMapHashFunc = u64(*)(void const* Key, usize KeySize);
    using tMapCompareFunc = int(*)(void const* KeyA, void const* KeyB, usize KeySize);

    template<typename K, typename V>
    struct tMap
    {
        /// May not be null.
        tAllocator Allocator;

        /// May not be null.
        tMapHashFunc HashFunc;

        /// May be null. Fallback is memcpy-style compairson.
        tMapCompareFunc CompareFunc;

        /// Number of elements in the map.
        ssize Count;

        /// Number of elements the map could theoretically store. The map is resized before this value is reached.
        ssize Capacity;

        /// Internal. Array(N=Capacity) of slots in this map.
        tMapSlot* Slots;

        /// Internal. Array(N=Capacity) of keys in this map.
        // #TODO Compute this based on Slots instead of storing it?
        K* Keys;

        /// Internal. Array(N=Capacity) of values in this map.
        // #TODO Compute this based on Slots instead of storing it?
        V* Values;
    };
}

namespace mtb
{
    template<typename K, typename V>
    tMapIterator_KeyOrValue<K> IterKeys(tMap<K, V>& Map);

    template<typename K, typename V>
    tMapIterator_KeyOrValue<V> IterValues(tMap<K, V>& Map);

    template<typename K, typename V>
    tMap<K, V> CreateMap(tAllocator Allocator, tMapHashFunc HashFunc, tMapCompareFunc CompareFunc = nullptr);

    template<typename K, typename V>
    void Put(tMap<K, V>& Map, K const& Key, V const& Value);

    template<typename K, typename V>
    V* Find(tMap<K, V>& Map, K const& Key);

    template<typename K, typename V>
    V& FindChecked(tMap<K, V>& Map, K const& Key);

    template<typename K, typename V>
    bool Remove(tMap<K, V>& Map, K const& Key);
}

namespace mtb
{
    template<typename K, typename V>
    void InternalMapPut(tMap<K, V>& Map, K const& Key, V const& Value);

    template<typename K, typename V>
    void InternalEnsureAdditionalCapacity(tMap<K, V>& Map, ssize AdditionalCount);
}

template<typename K, typename V>
void mtb::InternalMapPut(tMap<K, V>& Map, K const& Key, V const& Value)
{
    MTB_ASSERT(Map.Capacity > 0);

    tMapSlot* Slots = Map.Slots;
    K* Keys = Map.Keys;
    V* Values = Map.Values;
    ssize StartIndex = (u64)(Map.HashFunc(&Key, sizeof(K)) % Map.Capacity);
    ssize Index = StartIndex;
    bool bFound = false;
    while (true)
    {
        tMapSlot& Slot = Slots[Index];
        if (Slot.State == tMapSlot::kFree || Slot.State == tMapSlot::kDead)
        {
            Slot.State = tMapSlot::kOccupied;
            tItemOps<MTB_IS_POD(K)>::CopyConstruct(Keys + Index, 1, &Key, 1);
            tItemOps<MTB_IS_POD(K)>::CopyConstruct(Values + Index, 1, &Value, 1);
            ++Map.Count;
            bFound = true;
            break;
        }

        if (Map.CompareFunc(Keys + Index, &Key, sizeof(K)) == 0)
        {
            tItemOps<MTB_IS_POD(K)>::CopyConstruct(Values + Index, 1, &Value, 1);
            bFound = true;
            break;
        }

        if (++Index == Map.Capacity) Index = 0;
        if (Index == StartIndex) break;
    }

    MTB_ASSERT(bFound);
}

template<typename K, typename V>
void mtb::InternalEnsureAdditionalCapacity(tMap<K, V>& Map, ssize AdditionalCount)
{
    MTB_ASSERT(Map.Allocator);

    ssize Threshold = (ssize)(0.7f * Map.Capacity);
    if (Map.Count + AdditionalCount < Threshold)
    {
        // We got enough space.
        return;
    }

    ssize NewCapacity = Map.Capacity == 0 ? 64 : Map.Capacity << 1;
    usize const Alignment = MTB_ALIGNOF(V) > MTB_ALIGNOF(K) ? MTB_ALIGNOF(V) : MTB_ALIGNOF(K);
    usize const PayloadSize = sizeof(tMapSlot) + sizeof(K) + sizeof(V);
    tSlice<void> NewAllocation = AllocRaw(Map.Allocator, NewCapacity * PayloadSize, Alignment, /*bool bClearToZero =*/ true);

    tMap<K, V> NewMap{};
    NewMap.Allocator = Map.Allocator;
    NewMap.HashFunc = Map.HashFunc;
    NewMap.CompareFunc = Map.CompareFunc;
    NewMap.Count = 0;
    NewMap.Capacity = NewCapacity;
    NewMap.Slots = (tMapSlot*)NewAllocation.Ptr;
    NewMap.Keys = (K*)(NewMap.Slots + NewCapacity);
    NewMap.Values = (V*)(NewMap.Keys + NewCapacity);

    for (ssize Index = 0; Index < Map.Capacity; ++Index)
    {
        if (Map.Slots[Index].State == tMapSlot::kOccupied)
        {
            InternalMapPut(NewMap, Map.Keys[Index], Map.Values[Index]);
        }
    }

    tSlice<void> OldAllocation = PtrSlice((void*)Map.Slots, Map.Capacity * PayloadSize);
    DeallocRaw(Map.Allocator, OldAllocation, Alignment);

    Map = NewMap;
}

template<typename K, typename V>
mtb::tMapIterator_KeyOrValue<K> mtb::IterKeys(tMap<K, V>& Map)
{
    tMapIterator_KeyOrValue<K> Result;
    Result.Capacity = Map.Capacity;
    Result.Slots = Map.Slots;
    Result.Items = Map.Keys;
    return Result;
}

template<typename K, typename V>
mtb::tMapIterator_KeyOrValue<V> mtb::IterValues(tMap<K, V>& Map)
{
    tMapIterator_KeyOrValue<V> Result;
    Result.Capacity = Map.Capacity;
    Result.Slots = Map.Slots;
    Result.Items = Map.Values;
    return Result;
}

template<typename K, typename V>
mtb::tMap<K, V> mtb::CreateMap(tAllocator Allocator, tMapHashFunc HashFunc, tMapCompareFunc CompareFunc)
{
    MTB_ASSERT(Allocator);
    MTB_ASSERT(HashFunc);
    if(!CompareFunc)
    {
        CompareFunc = MTB_memcmp;
    }
    return{ Allocator, HashFunc, CompareFunc };
}

template<typename K, typename V>
void mtb::Put(tMap<K, V>& Map, K const& Key, V const& Value)
{
    InternalEnsureAdditionalCapacity(Map, 1);
    InternalMapPut(Map, Key, Value);
}

template<typename K, typename V>
V* mtb::Find(tMap<K, V>& Map, K const& Key)
{
    V* Result = nullptr;
    if(Map.Count)
    {
        ssize StartIndex = (u64)(Map.HashFunc(&Key, sizeof(K)) % Map.Capacity);
        ssize Index = StartIndex;
        while(Map.Slots[Index].State != tMapSlot::kFree)
        {
            tMapSlot& Slot = Map.Slots[Index];
            if(Slot.State == tMapSlot::kFree) break;

            if(Slot.State == tMapSlot::kOccupied && Map.CompareFunc(Map.Keys + Index, &Key, sizeof(K)) == 0)
            {
                Result = Map.Values + Index;
                break;
            }

            if(++Index == Map.Capacity) Index = 0;
            if(Index == StartIndex) break;
        }
    }

    return Result;
}

template<typename K, typename V>
V& mtb::FindChecked(tMap<K, V>& Map, K const& Key)
{
    V* Value = Find(Map, Key);
    MTB_ASSERT(Value);
    return *Value;
}

template<typename K, typename V>
bool mtb::Remove(tMap<K, V>& Map, K const& Key)
{
    bool bResult = false;
    if (Map.Count)
    {
        ssize StartIndex = (u64)(Map.HashFunc(&Key, sizeof(K)) % Map.Capacity);
        ssize Index = StartIndex;
        while(true)
        {
            tMapSlot& Slot = Map.Slots[Index];
            if (Slot.State == tMapSlot::kFree) break;

            if (Map.Slots[Index].State == tMapSlot::kOccupied && Map.CompareFunc(Map.Keys + Index, &Key, sizeof(K)) == 0)
            {
                Map.Slots[Index].State = tMapSlot::kDead;
                tItemOps<MTB_IS_POD(K)>::Destruct(Map.Keys + Index, 1);
                tItemOps<MTB_IS_POD(K)>::Destruct(Map.Values + Index, 1);
                bResult = true;
                break;
            }

            if (++Index == Map.Capacity) Index = 0;
            if (Index == StartIndex) break;
        }
    }

    return bResult;
}

//
// #Section Delegate
//
namespace mtb
{
    template<typename>
    struct tDelegate;

    template<typename R, typename... tArgs>
    struct tDelegate<R(tArgs...)>
    {
        using tDispatchSignature = R(void*, tArgs&&...);
        using tDelegateSignature = R(tArgs&&...);

        void* DispatchTarget{};
        tDispatchSignature* DispatchProc{};

        template<typename tTargetSignature>
        MTB_inline static R Dispatcher(void* DispatchTarget, tArgs&&... Args)
        {
            return (*(tTargetSignature*)DispatchTarget)(ForwardCast<tArgs>(Args)...);
        }

        tDelegate() {}

        tDelegate(tDelegateSignature* PlainProcPtr)
        : DispatchTarget{PlainProcPtr}
        , DispatchProc{Dispatcher<tDelegateSignature>}
        {
            MTB_ASSERT(PlainProcPtr);
        }

        template<typename T>
        tDelegate(T&& CallableTarget)
        : DispatchTarget{&CallableTarget}
        , DispatchProc{Dispatcher<tDecay<T>>}
        {
        }

        MTB_inline R operator()(tArgs&&... Args)
        {
            MTB_ASSERT(DispatchProc && DispatchTarget);
            return DispatchProc(DispatchTarget, ForwardCast<tArgs>(Args)...);
        }

        MTB_inline constexpr explicit operator bool() const
        {
            return !!DispatchTarget && !!DispatchProc;
        }
    };
} // namespace mtb

//
// #Section Sorting
//
namespace mtb
{
    // Quick sort sorting algorithm. This procedure does not require to know the actual data. It relies on the fact that
    // LessProc and SwapProc are able to produce the desired information/effect required for effectively sorting
    // something. This implementation works on all data structures that work using indices. In other words, you can sort
    // data in linked lists if you can index each node, or sort a discontiguous (chunked) array.
    //
    // \remark This sort is not stable.
    //
    // \param UserPtr will be passed through to \a LessProc and \a SwapProc as-is. Can be null.
    // \param Count The number of items to sort. In other words, this is the first invalid index into whatever is being sorted.
    // \param LessProc Whether element at the first index is considered less than the element at the second index.
    // \param Threshold Partitions with a count equal to or less than this value will be sorted with insertion sort.
    void QuickSort(void* UserPtr, ssize Count, bool (*LessProc)(void*, ssize, ssize), void (*SwapProc)(void*, ssize, ssize), ssize Threshold);

    template<typename T, typename tLessProc, typename tSwapProc>
    MTB_inline void QuickSortSlice(tSlice<T> Slice, tLessProc LessProc, tSwapProc SwapProc, ssize Threshold = 16)
    {
        struct tContext
        {
            T* Ptr;
            tLessProc Less;
            tSwapProc Swap;
        } Context{Slice.Ptr, LessProc, SwapProc};
        QuickSort(&Context,
                  Slice.Count,
                  [](void* C, ssize I, ssize J) {
                      tContext* Context = (tContext*)C;
                      return Context->Less(Context->Ptr[I], Context->Ptr[J]);
                  },
                  [](void* C, ssize I, ssize J) {
                      tContext* Context = (tContext*)C;
                      Context->Swap(Context->Ptr[I], Context->Ptr[J]);
                  },
                  Threshold);
    }

    template<typename T, typename tLessProc>
    MTB_inline void QuickSortSlice(tSlice<T> Slice, tLessProc LessProc, ssize Threshold = 16)
    {
        struct tContext
        {
            T* Ptr;
            tLessProc Less;
        } Context{Slice.Ptr, LessProc};
        QuickSort(&Context,
                  Slice.Count,
                  [](void* C, ssize I, ssize J) {
                      tContext* Context = (tContext*)C;
                      return Context->Less(Context->Ptr[I], Context->Ptr[J]);
                  },
                  [](void* C, ssize I, ssize J) {
                      tContext* Context = (tContext*)C;
                      ::mtb::Swap(Context->Ptr[I], Context->Ptr[J]);
                  },
                  Threshold);
    }

    template<typename T>
    MTB_inline void QuickSortSlice(tSlice<T> Slice, ssize Threshold = 16)
    {
        QuickSort(Slice.Ptr,
                  Slice.Count,
                  [](void* Ptr, ssize I, ssize J) { return ((T*)Ptr)[I] < ((T*)Ptr)[J]; },
                  [](void* Ptr, ssize I, ssize J) { ::mtb::Swap(((T*)Ptr)[I], ((T*)Ptr)[J]); },
                  Threshold);
    }
} // namespace mtb

// #Note I tried using tOption. In general, I would like something like that
// very much. However, it's so hard to correctly implement in C++ that I don't
// think it's worth the effort. One would have to verify on all compilers that
// it actually behaves as expected with regards to implicit casting and move
// semantics, and that it doesn't doesn't allow implicit casts from T to
// tOption<T> etc. There's too much uncertainty on little details that could
// go wrong. So I just disable this here. Maybe at some point in the future
// (C++20 and beyond?) it might be easier to achieve this kind of thing.
#if 0
//
// #Section Option
//
namespace mtb::option
{
    template<typename T>
    struct tOption
    {
        bool HasValue;
        union {
            T Value;
        };

        MTB_inline tOption()
        : HasValue{false} {}

        MTB_inline tOption(T InValue)
        : HasValue{true}, Value{MoveCast(InValue)} {}

        MTB_inline tOption(T&& InValue)
        : HasValue{true}, Value{MoveCast(InValue)} {}

        MTB_inline tOption(tOption const& ToCopy)
        : HasValue{ToCopy.HasValue}
        {
            if (ToCopy.HasValue)
            {
                new (&Value) T(ToCopy.Value);
            }
        }

        MTB_inline tOption(tOption&& ToMove)
        : HasValue{ToMove.HasValue}
        {
            if (ToMove.HasValue)
            {
                new (&Value) T(MoveCast(ToMove.Value));
            }
        }

        MTB_inline ~tOption()
        {
            if (HasValue)
            {
                Value.~T();
            }
        }

        tOption& operator=(tOption const& ToCopy)
        {
            if (&ToCopy != this)
            {
                if (!HasValue && !ToCopy.HasValue)
                {
                    // value stays untouched.
                }
                else if (HasValue && ToCopy.HasValue)
                {
                    // we had a value, so copy directly.
                    Value = ToCopy.Value;
                }
                else if (!HasValue && ToCopy.HasValue)
                {
                    // we didn't have a value before, so construct.
                    new (&Value) T(ToCopy.Value);
                    HasValue = true;
                }
                else if (HasValue && !ToCopy.HasValue)
                {
                    // we had a value, so destroy.
                    Value.~T();
                    HasValue = false;
                }
            }

            return *this;
        }

        tOption& operator=(tOption&& ToMove)
        {
            if (&ToMove != this)
            {
                if (!HasValue && !ToMove.HasValue)
                {
                    // value stays untouched.
                }
                else if (HasValue && ToMove.HasValue)
                {
                    // we had a value, so copy directly.
                    Value = (T &&) ToMove.Value;
                }
                else if (!HasValue && ToMove.HasValue)
                {
                    // we didn't have a value before, so construct.
                    new (&Value) T((T &&) ToMove.Value);
                    HasValue = true;
                }
                else if (HasValue && !ToMove.HasValue)
                {
                    // we had a value, so destroy.
                    Value.~T();
                    HasValue = false;
                }
            }

            return *this;
        }

        constexpr operator bool() const { return HasValue; }
    };

    template<typename T>
    constexpr bool operator==(tOption<T> const& A, tOption<T> const& B)
    {
        return (!A.HasValue && !B.HasValue) || (A.HasValue && B.HasValue && A.Value == B.Value);
    }

    template<typename T>
    constexpr bool operator!=(tOption<T> const& A, tOption<T> const& B)
    {
        return !(A == B);
    }

    template<typename T>
    T& Unwrap(tOption<T>& Option)
    {
        MTB_ASSERT(Option.HasValue);
        return Option.Value;
    }

    template<typename T>
    T Unwrap(tOption<T> Option)
    {
        MTB_ASSERT(Option.HasValue);
        return Option.Value;
    }
} // namespace mtb::option
#endif

//
// #Section Arena
//

// #Option
#if !defined(MTB_ARENA_DEFAULT_BUCKET_SIZE)
#define MTB_ARENA_DEFAULT_BUCKET_SIZE 4096
#endif

namespace mtb::arena
{
    struct tBucket
    {
        tBucket* Next;
        tBucket* Prev;
        usize UsedSize;
        usize TotalSize;
        u8 Data[1]; // trailing data
    };

    struct tArenaMarker
    {
        tBucket* Bucket;
        usize Offset;

        MTB_inline constexpr u8* Ptr()
        {
            return Bucket ? Bucket->Data + Offset : nullptr;
        }
    };

    // #TODO struct tArenaStats { usize LargestBucketSize; usize NumBucketAllocations; usize NumBucketFrees; };

    struct tArena
    {
        tAllocator ChildAllocator;
        usize MinBucketSize;
        tBucket* CurrentBucket;
        tBucket* FirstFreeBucket;

        usize LargestBucketSize;
    };

    namespace eArenaInit
    {
        enum Type
        {
            kNoInit,
            kClearToZero,
            kDefaultConstruct,
        };
    }

    usize BucketTotalSize(tBucket* Bucket);

    usize BucketUsedSize(tBucket* Bucket);

    tAllocator GetChildAllocator(tArena& Arena);

    void Reserve(tArena& Arena, usize TotalSize);

    void Grow(tArena& Arena, usize RequiredSize);

    void Clear(tArena& Arena, bool bReleaseMemory = true);

    void* ReallocRaw(tArena& Arena, void* OldPtr, usize OldSize, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero = true);

    void* PushRaw(tArena& Arena, usize Size, usize Alignment, bool bClearToZero = true);

    tSlice<void> ReallocRawArray(tArena& Arena, tSlice<void> OldArray, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero = true);

    tSlice<void> PushRawArray(tArena& Arena, usize Size, usize Alignment, bool bClearToZero = true);

    template<typename T>
    tSlice<T> ReallocArray(tArena& Arena, tSlice<T> OldArray, usize NewCount, eArenaInit::Type Init = eArenaInit::kDefaultConstruct)
    {
        usize NewSize = MTB_SIZEOF(T) * NewCount;
        usize Alignment = MTB_ALIGNOF(T);
        void* Ptr = ReallocRaw(Arena, OldArray.Ptr, SliceSize(OldArray), Alignment, NewSize, Alignment, Init == eArenaInit::kClearToZero);
        tSlice<T> Result = PtrSlice((T*)Ptr, NewCount);
        if (Init == eArenaInit::kDefaultConstruct)
        {
            SliceDefaultConstructItems(SliceOffset(Result, OldArray.Count));
        }
        return Result;
    }

    template<typename T>
    tSlice<T> PushArray(tArena& Arena, usize Count, eArenaInit::Type Init = eArenaInit::kDefaultConstruct)
    {
        return ReallocArray<T>(Arena, {}, Count, Init);
    }

    template<typename T, typename U = tRemoveConst<T>>
    tSlice<U> PushCopyArray(tArena& Arena, tSlice<T> ToCopy)
    {
        tSlice<U> Copy = PushArray<U>(Arena, ToCopy.Count, eArenaInit::kNoInit);
        SliceCopyConstructItems(Copy, ToCopy);
        return Copy;
    }

    // The same as PushCopyArray with one additional zero-element at the end.
    template<typename T, typename U = tRemoveConst<T>>
    tSlice<U> PushCopyString(tArena& Arena, tSlice<T> ToCopy)
    {
        tSlice<U> ZeroTerminatedCopy = PushArray<U>(Arena, ToCopy.Count + 1, eArenaInit::kNoInit);
        tSlice<U> Result = SliceRange(ZeroTerminatedCopy, 0, ZeroTerminatedCopy.Count - 1);
        SliceCopyConstructItems(Result, ToCopy);
        // set terminator element.
        new (PtrOffset(ZeroTerminatedCopy.Ptr, ZeroTerminatedCopy.Count - 1)) U{};
        return Result;
    }

    template<typename T>
    T* PushOne(tArena& Arena, eArenaInit::Type Init = eArenaInit::kDefaultConstruct)
    {
        return PushArray<T>(Arena, 1, Init).Ptr;
    }

    template<typename T>
    T* PushCopy(tArena& Arena, T const& Object)
    {
        T* Result = PushOne<T>(Arena, eArenaInit::kNoInit);
        SliceCopyConstructItems(PtrSlice(Result, 1), PtrSlice(&Object, 1));
        return Result;
    }

    /* #TODO
     - NumBuckets?
     - accumulated TotalSize across all buckets?
     - accumulated UsedSize across all buckets?
    */

    /// \remark Only valid before free was called.
    tArenaMarker GetMarker(tArena& Arena);
    void ResetToMarker(tArena& Arena, tArenaMarker Marker, bool bReleaseMemory = true);

    /// Ensure the memory in the given range is contiguous.
    /// Returns the marker to the beginning of the linearized section.
    tSlice<void> Linearize(tArena& Arena, tArenaMarker Begin, tArenaMarker End);

    tAllocator MakeAllocator(tArena& Arena);

#if MTB_USE_STB_SPRINTF
    /// \brief Produce several fragments of formatted strings within the given arena. Use `*printf_Arena` or `Linearize` to produce an actual string.
    void vprintf_ArenaRaw(tArena& Arena, char const* Format, va_list VArgs);

    // Append a string with the given format inside the given arena.
    MTB_inline void printf_ArenaRaw(tArena& Arena, char const* Format, ...)
    {
        va_list Args;
        va_start(Args, Format);
        vprintf_ArenaRaw(Arena, Format, Args);
        va_end(Args);
    }

    /// \brief Append a string with the given format inside the given arena. Produces a null-terminated string.
    /// \return The formatted string EXCLUDING the null-terminator.
    tSlice<char> vprintf_Arena(tArena& Arena, char const* Format, va_list VArgs);

    // Append a string with the given format inside the given arena.
    MTB_inline tSlice<char> printf_Arena(tArena& Arena, char const* Format, ...)
    {
        va_list Args;
        va_start(Args, Format);
        tSlice<char> Result = vprintf_Arena(Arena, Format, Args);
        va_end(Args);

        return Result;
    }

#endif // MTB_USE_STB_SPRINTF

} // namespace mtb::arena

//
// #Section Strings
//
namespace mtb::string
{
    // #TODO I want UTF-8 support :(

    enum struct eStringComparison
    {
        kCaseSensitive,
        kIgnoreCase,
    };

    char ToLowerChar(char Char);
    char ToUpperChar(char Char);

    bool IsDigitChar(char Char);
    bool IsWhiteChar(char Char);

    s32 StringCompare(tSlice<char const> StrA, tSlice<char const> StrB, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringEquals(tSlice<char const> StrA, tSlice<char const> StrB, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringStartsWith(tSlice<char const> Str, tSlice<char const> Prefix, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringEndsWith(tSlice<char const> Str, tSlice<char const> Prefix, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    using tTrimPredicate = bool (*)(char);

    tSlice<char const> StringTrimStartPredicate(tSlice<char const> Str, tTrimPredicate Predicate);
    MTB_inline tSlice<char> StringTrimStartPredicate(tSlice<char> Str, tTrimPredicate Predicate) { return SliceRemoveConst(StringTrimStartPredicate((tSlice<char const>)Str, Predicate)); }

    tSlice<char const> StringTrimEndPredicate(tSlice<char const> Str, tTrimPredicate Predicate);
    MTB_inline tSlice<char> StringTrimEndPredicate(tSlice<char> Str, tTrimPredicate Predicate) { return SliceRemoveConst(StringTrimEndPredicate((tSlice<char const>)Str, Predicate)); }

    tSlice<char const> StringTrimPredicate(tSlice<char const> Str, tTrimPredicate Predicate);
    MTB_inline tSlice<char> StringTrimPredicate(tSlice<char> Str, tTrimPredicate Predicate) { return SliceRemoveConst(StringTrimPredicate((tSlice<char const>)Str, Predicate)); }

    tSlice<char const> StringTrimStart(tSlice<char const> Str);
    MTB_inline tSlice<char> StringTrimStart(tSlice<char> Str) { return SliceRemoveConst(StringTrimStart((tSlice<char const>)Str)); }

    tSlice<char const> StringTrimEnd(tSlice<char const> Str);
    MTB_inline tSlice<char> StringTrimEnd(tSlice<char> Str) { return SliceRemoveConst(StringTrimEnd((tSlice<char const>)Str)); }

    tSlice<char const> StringTrim(tSlice<char const> Str);
    MTB_inline tSlice<char> StringTrim(tSlice<char> Str) { return SliceRemoveConst(StringTrim((tSlice<char const>)Str)); }

    tSlice<char> AllocString(tAllocator A, usize CharCount);
    tSlice<char> AllocStringCopy(tAllocator A, tSlice<char const> String);
    void DeallocString(tAllocator A, tSlice<char> String);

    //
    // Wide char overloads
    //
    wchar_t ToLowerChar(wchar_t Char);
    wchar_t ToUpperChar(wchar_t Char);

    bool IsDigitChar(wchar_t Char);
    bool IsWhiteChar(wchar_t Char);

    s32 StringCompare(tSlice<wchar_t const> StrA, tSlice<wchar_t const> StrB, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringEquals(tSlice<wchar_t const> StrA, tSlice<wchar_t const> StrB, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringStartsWith(tSlice<wchar_t const> Str, tSlice<wchar_t const> Prefix, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    bool StringEndsWith(tSlice<wchar_t const> Str, tSlice<wchar_t const> Prefix, eStringComparison Cmp = eStringComparison::kCaseSensitive);

    using tWTrimPredicate = bool (*)(wchar_t);

    tSlice<wchar_t const> StringTrimStartPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate);
    MTB_inline tSlice<wchar_t> StringTrimStartPredicate(tSlice<wchar_t> Str, tWTrimPredicate Predicate) { return SliceRemoveConst(StringTrimStartPredicate((tSlice<wchar_t const>)Str, Predicate)); }

    tSlice<wchar_t const> StringTrimEndPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate);
    MTB_inline tSlice<wchar_t> StringTrimEndPredicate(tSlice<wchar_t> Str, tWTrimPredicate Predicate) { return SliceRemoveConst(StringTrimEndPredicate((tSlice<wchar_t const>)Str, Predicate)); }

    tSlice<wchar_t const> StringTrimPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate);
    MTB_inline tSlice<wchar_t> StringTrimPredicate(tSlice<wchar_t> Str, tWTrimPredicate Predicate) { return SliceRemoveConst(StringTrimPredicate((tSlice<wchar_t const>)Str, Predicate)); }

    tSlice<wchar_t const> StringTrimStart(tSlice<wchar_t const> Str);
    MTB_inline tSlice<wchar_t> StringTrimStart(tSlice<wchar_t> Str) { return SliceRemoveConst(StringTrimStart((tSlice<wchar_t const>)Str)); }

    tSlice<wchar_t const> StringTrimEnd(tSlice<wchar_t const> Str);
    MTB_inline tSlice<wchar_t> StringTrimEnd(tSlice<wchar_t> Str) { return SliceRemoveConst(StringTrimEnd((tSlice<wchar_t const>)Str)); }

    tSlice<wchar_t const> StringTrim(tSlice<wchar_t const> Str);
    MTB_inline tSlice<wchar_t> StringTrim(tSlice<wchar_t> Str) { return SliceRemoveConst(StringTrim((tSlice<wchar_t const>)Str)); }

    tSlice<wchar_t> AllocWString(tAllocator A, usize CharCount);
    tSlice<wchar_t> AllocStringCopy(tAllocator A, tSlice<wchar_t const> String);
    void DeallocWString(tAllocator A, tSlice<wchar_t> String);

    //
    // C-String interop (char)
    //
    usize StringLengthZ(char const* StrZ);

    MTB_inline tSlice<char> WrapZ(char* StrZ) { return PtrSlice(StrZ, StringLengthZ(StrZ)); }

    MTB_inline tSlice<char const> ConstZ(char const* StrZ) { return PtrSlice(StrZ, StringLengthZ(StrZ)); }

    //
    // C-String interop (wchar_t)
    //
    usize StringLengthZ(wchar_t const* StrZ);

    MTB_inline tSlice<wchar_t> WrapZ(wchar_t* StrZ) { return PtrSlice(StrZ, StringLengthZ(StrZ)); }

    MTB_inline tSlice<wchar_t const> ConstZ(wchar_t const* StrZ) { return PtrSlice(StrZ, StringLengthZ(StrZ)); }
} // namespace mtb::string

//
// #Section Units
//
namespace mtb
{
    const u64 kKibiBytesToBytes = 1024ULL;
    const u64 kMebiBytesToBytes = 1024ULL * kKibiBytesToBytes;
    const u64 kGibiBytesToBytes = 1024ULL * kMebiBytesToBytes;
    const u64 kTebiBytesToBytes = 1024ULL * kGibiBytesToBytes;
    const u64 kPebiBytesToBytes = 1024ULL * kTebiBytesToBytes;
    const u64 kExbiBytesToBytes = 1024ULL * kPebiBytesToBytes;

    const u64 kKiloBytesToBytes = 1000ULL;
    const u64 kMegaBytesToBytes = 1000ULL * kKiloBytesToBytes;
    const u64 kGigaBytesToBytes = 1000ULL * kMegaBytesToBytes;
    const u64 kTeraBytesToBytes = 1000ULL * kGigaBytesToBytes;
    const u64 kPetaBytesToBytes = 1000ULL * kTeraBytesToBytes;
    const u64 kExaBytesToBytes = 1000ULL * kPetaBytesToBytes;

    // clang-format off
    const u64 kMicrosecondsToNanoseconds = 1000ULL;
    const u64 kMillisecondsToNanoseconds = 1000ULL * kMicrosecondsToNanoseconds;
    const u64 kSecondsToNanoseconds      = 1000ULL * kMillisecondsToNanoseconds;
    const u64 kMinutesToNanoseconds      =   60ULL * kSecondsToNanoseconds;
    const u64 kHoursToNanoseconds        =   60ULL * kMinutesToNanoseconds;
    const u64 kDaysToNanoseconds         =   24ULL * kHoursToNanoseconds;
    // clang-format on

    /// Break a value into whole unit components.
    /// \example
    ///    u64 ByteValue = (1337 * kMebiBytesToBytes) + (666 * kKibiBytesToBytes) + 42;
    ///    u64 Units[]{kMebiBytesToBytes, kKibiBytesToBytes, 1}; u32 Results[3];
    ///    BreakIntoUnits(ByteValue, ArraySlice(Units), ArraySlice(Results));
    ///    // Results[0] == 1337; Results[1] == 666; Results[0] == 42;
    void BreakIntoUnits(u64 Value, tSlice<u64 const> UnitsTable, tSlice<u32> out_Results);
} // namespace mtb

//
// #Section Timespan ClockTime
//

namespace mtb::time
{
    struct tTimespan
    {
        s64 Nanoseconds;

        MTB_inline constexpr operator bool() const { return (bool)Nanoseconds; }
    };

    constexpr tTimespan kTimespanZero{};
    constexpr tTimespan kTimespanMin{tIntProperties<s64>::MinValue};
    constexpr tTimespan kTimespanMax{tIntProperties<s64>::MaxValue};

    //
    // timespan + timespan
    //
    MTB_inline constexpr tTimespan operator+(tTimespan a, tTimespan b)
    {
        return {a.Nanoseconds + b.Nanoseconds};
    }
    MTB_inline constexpr tTimespan& operator+=(tTimespan& a, tTimespan b)
    {
        a.Nanoseconds += b.Nanoseconds;
        return a;
    }

    //
    // (timespan) - timespan
    //
    MTB_inline constexpr tTimespan operator-(tTimespan a)
    {
        return {-a.Nanoseconds};
    }
    MTB_inline constexpr tTimespan operator-(tTimespan a, tTimespan b)
    {
        return {a.Nanoseconds - b.Nanoseconds};
    }
    MTB_inline constexpr tTimespan& operator-=(tTimespan& a, tTimespan b)
    {
        a.Nanoseconds -= b.Nanoseconds;
        return a;
    }

    //
    // timespan comparison
    //
    MTB_inline constexpr bool operator==(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds == b.Nanoseconds;
    }
    MTB_inline constexpr bool operator!=(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds != b.Nanoseconds;
    }
    MTB_inline constexpr bool operator<(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds < b.Nanoseconds;
    }
    MTB_inline constexpr bool operator<=(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds <= b.Nanoseconds;
    }
    MTB_inline constexpr bool operator>(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds > b.Nanoseconds;
    }
    MTB_inline constexpr bool operator>=(tTimespan a, tTimespan b)
    {
        return a.Nanoseconds >= b.Nanoseconds;
    }

    // clang-format off
    MTB_inline constexpr s64 GetNanoseconds(tTimespan Timespan)  { return Timespan.Nanoseconds; }
    MTB_inline constexpr f64 GetMicroseconds(tTimespan Timespan) { return (f64)Timespan.Nanoseconds / (f64)kMicrosecondsToNanoseconds; }
    MTB_inline constexpr f64 GetMilliseconds(tTimespan Timespan) { return (f64)Timespan.Nanoseconds / (f64)kMillisecondsToNanoseconds; }
    MTB_inline constexpr f64 GetSeconds(tTimespan Timespan)      { return (f64)Timespan.Nanoseconds / (f64)kSecondsToNanoseconds; }
    MTB_inline constexpr f64 GetMinutes(tTimespan Timespan)      { return (f64)Timespan.Nanoseconds / (f64)kMinutesToNanoseconds; }
    MTB_inline constexpr f64 GetHours(tTimespan Timespan)        { return (f64)Timespan.Nanoseconds / (f64)kHoursToNanoseconds; }
    MTB_inline constexpr f64 GetDays(tTimespan Timespan)         { return (f64)Timespan.Nanoseconds / (f64)kDaysToNanoseconds; }
    // clang-format on

    // clang-format off
    MTB_inline constexpr tTimespan FromNanoseconds(s64 in_Nanoseconds)   { return { in_Nanoseconds }; }
    MTB_inline constexpr tTimespan FromMicroseconds(f64 in_Microseconds) { return { (s64)(in_Microseconds * (f64)kMicrosecondsToNanoseconds) }; }
    MTB_inline constexpr tTimespan FromMilliseconds(f64 in_Milliseconds) { return { (s64)(in_Milliseconds * (f64)kMillisecondsToNanoseconds) }; }
    MTB_inline constexpr tTimespan FromSeconds     (f64 in_Seconds)      { return { (s64)(in_Seconds      * (f64)kSecondsToNanoseconds) }; }
    MTB_inline constexpr tTimespan FromMinutes     (f64 in_Minutes)      { return { (s64)(in_Minutes      * (f64)kMinutesToNanoseconds) }; }
    MTB_inline constexpr tTimespan FromHours       (f64 in_Hours)        { return { (s64)(in_Hours        * (f64)kHoursToNanoseconds) }; }
    MTB_inline constexpr tTimespan FromDays        (f64 in_Days)         { return { (s64)(in_Days         * (f64)kDaysToNanoseconds) }; }
    // clang-format on

    struct tClockTime
    {
        bool negative;
        u32 days;
        u32 hours;
        u32 minutes;
        u32 seconds;
        u32 milliseconds;
        u32 microseconds;
        u32 nanoseconds;
    };

    tClockTime ClockTimeFromTimespan(tTimespan span);
} // namespace mtb::time

#endif // !defined(MTB__INCLUDED)

//
// #Section Implementation
//
#if defined(MTB_IMPLEMENTATION)

void mtb::impl::CopyRaw(void* Destination, void const* Source, usize Size)
{
#if MTB_USE_LIBC
    ::memcpy(Destination, Source, Size);
#else
    u8* Dst = (u8*)Destination;
    u8 const* Src = (u8 const*)Source;
    for (usize Index = 0; Index < Size; ++Index)
    {
        Dst[Index] = Src[Index];
    }
#endif // MTB_USE_LIBC
}

void mtb::impl::MoveRaw(void* Destination, void const* Source, usize Size)
{
#if MTB_USE_LIBC
    ::memmove(Destination, Source, Size);
#else
    u8* Dst = (u8*)Destination;
    u8 const* Src = (u8 const*)Source;
    if (Dst < Src)
    {
        // copy forward
        for (usize Index = 0; Index < Size; ++Index)
        {
            Dst[Index] = Src[Index];
        }
    }
    else
    {
        // copy reverse
        for (usize rIndex = Size; rIndex > 0; --rIndex)
        {
            usize Index = rIndex - 1;
            Dst[Index] = Src[Index];
        }
    }
#endif // MTB_USE_LIBC
}

void mtb::impl::SetRaw(void* Destination, int ByteValue, usize Size)
{
#if MTB_USE_LIBC
    ::memset(Destination, ByteValue, Size);
#else
    u8* Dst = (u8*)Destination;
    u8 ByteValue8 = (u8)ByteValue;
    for (usize Index = 0; Index < Size; Index++)
    {
        Dst[Index] = ByteValue8;
    }
#endif
}

int mtb::impl::CompareRaw(void const* A, void const* B, usize Size)
{
#if MTB_USE_LIBC && 0
    return ::memcmp(A, B, Size);
#else
    u8* ByteA = (u8*)A;
    u8* ByteB = (u8*)B;
    for(usize Index = 0; Index < Size; ++Index)
    {
        if(ByteA[Index] != ByteB[Index])
        {
            return ByteA[Index] - ByteB[Index];
        }
    }
    return 0;
#endif
}

int mtb::SliceCompareRaw(tSlice<void const> A, tSlice<void const> B)
{
    int Result = (int)(A.Count - B.Count);
    if (Result == 0)
    {
        Result = MTB_memcmp(A.Ptr, B.Ptr, SliceSize(A));
    }
    return Result;
}

bool mtb::SliceEqualsRaw(tSlice<void const> A, tSlice<void const> B)
{
    return SliceCompareRaw(A, B) == 0;
}

void mtb::AlignAllocation(void** inout_Ptr, usize* inout_Size, usize Alignment)
{
    MTB_ASSERT(inout_Ptr != nullptr);
    MTB_ASSERT(*inout_Ptr != nullptr);
    MTB_ASSERT(Alignment > 0);

    uptr Unaligned = (uptr)*inout_Ptr;

    /* Example
        Unaligned =      0b1010'1010 // input pointer
        Alignment = 16 = 0b0001'0000

        A = 0b0000'1111
        ~A = 0b1111'0000

        Unaligned + A = 0b1011'1001
        (Unaligned + A) & ~A = 0b1011'0000 // i.e. the last four bits are cleared.
    */
    usize A = Alignment - 1;
    uptr Aligned = (Unaligned + A) & ~A;
    *inout_Ptr = (void*)Aligned;
    if(inout_Size)
    {
        *inout_Size += Aligned - Unaligned;
    }
}

// IMPL
namespace mtb::impl
{
    usize ChooseAlignment(tAllocator A, usize Alignment)
    {
        usize Result = Alignment;
        if(Result == 0)
        {
            Result = A.DefaultAlignment;
        }
        if(Result == 0)
        {
            Result = MTB_ALLOCATOR_DEFAULT_ALIGNMENT;
        }
        return Result;
    }
}

mtb::tSlice<void> mtb::AllocRaw(tAllocator A, usize Size, usize Alignment, bool bClearToZero /* = true */)
{
    MTB_ASSERT(A);
    usize ChosenAlignment = impl::ChooseAlignment(A, Alignment);
    tSlice<void> Result = A.ReallocProc(A.User, {}, ChosenAlignment, Size, ChosenAlignment, bClearToZero);
    return Result;
}

void mtb::DeallocRaw(tAllocator A, tSlice<void> Mem, usize Alignment /* = 0 */)
{
    MTB_ASSERT(A);
    usize ChosenAlignment = impl::ChooseAlignment(A, Alignment);
    A.ReallocProc(A.User, Mem, ChosenAlignment, 0, ChosenAlignment, false);
}

mtb::tSlice<void> mtb::ReallocRaw(tAllocator A, tSlice<void> OldMem, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero /* = true */)
{
    MTB_ASSERT(A);
    return A.ReallocProc(A.User, OldMem, impl::ChooseAlignment(A, OldAlignment), NewSize, impl::ChooseAlignment(A, NewAlignment), bClearToZero);
}

mtb::tAllocator mtb::GetDefaultAllocator()
{
#if MTB_USE_LIBC
    return GetLibcAllocator();
#else
    MTB_ASSERT(!"No default allocator available without using libc.");
    return {};
#endif
}

#if MTB_USE_LIBC
namespace mtb::impl
{
    tSlice<void> LibcReallocProc(void* User, tSlice<void> OldMem, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero)
    {
        MTB_ASSERT(OldAlignment <= NewAlignment && "Changing alignment is not supported");

        tSlice<void> Result{};
        if(OldMem || NewSize)
        {
            void* NewPtr = ::realloc(OldMem.Ptr, NewSize);
            if(NewPtr)
            {
                Result = PtrSlice(NewPtr, NewSize);
                void* AlignedPtr = Result.Ptr;
                usize AlignedSize = Result.Count;
                AlignAllocation(&AlignedPtr, &AlignedSize, NewAlignment);
                MTB_ASSERT(AlignedSize == NewSize && "Unsupported alignment");

                if(bClearToZero && OldMem.Count < Result.Count)
                {
                    SliceDefaultConstructItems(SliceOffset(Result, OldMem.Count));
                }
            }
            else
            {
                MTB_ASSERT((OldMem.Ptr == nullptr || NewSize == 0) && "realloc failed to resize an existing allocation?!");
            }
        }

        return Result;
    }
} // namespace mtb::impl

mtb::tAllocator mtb::GetLibcAllocator()
{
    tAllocator Result{};
    Result.ReallocProc = impl::LibcReallocProc;
    return Result;
}
#endif

#if MTB_USE_STB_SPRINTF
namespace mtb
{
    char* InternalPrintfCallback(char const* buf, void* user, int len)
    {
        PushMany(*(tArray<char>*)user, PtrSlice(buf, len));
        return (char*)buf;
    }
} // namespace mtb

mtb::tSlice<char> mtb::vprintfAlloc(tAllocator A, char const* Format, va_list VArgs)
{
    tArray<char> Buffer{A};
    char TempBuffer[STB_SPRINTF_MIN];
    stbsp_vsprintfcb(InternalPrintfCallback, &Buffer, TempBuffer, Format, VArgs);
    return ShrinkAllocation(Buffer);
}

mtb::tSlice<char> mtb::PushFormat(tArray<char>& Array, char const* Format, ...)
{
    va_list VArgs;
    va_start(VArgs, Format);
    tSlice<char> Result = PushFormatV(Array, Format, VArgs);
    va_end(VArgs);
    return Result;
}

mtb::tSlice<char> mtb::PushFormatV(tArray<char>& Array, char const* Format, va_list VArgs)
{
    ssize Offset = Array.Count;
    char TempBuffer[STB_SPRINTF_MIN];
    stbsp_vsprintfcb(InternalPrintfCallback, &Array, TempBuffer, Format, VArgs);
    return SliceOffset(Array.Slice, Offset);
}
#endif // MTB_USE_STB_SPRINTF

mtb::tSlice<char> mtb::ToString(tArray<char> Array, bool bNullTerminate /*= true*/)
{
    ssize EndOffset = 0;
    if (bNullTerminate)
    {
        Push(Array, '\0');
        EndOffset = 1;
    }
    ShrinkAllocation(Array);
    return SliceCast<char>(SliceRange(Array.Slice, 0, Array.Count - EndOffset));
}

void mtb::QuickSort(void* UserPtr, ssize Count, bool(*LessProc)(void*, ssize, ssize), void(*SwapProc)(void*, ssize, ssize), ssize Threshold)
{
    // Based on https://github.com/svpv/qsort
    if (Count > 1)
    {
        ssize Left = 0;
        ssize Right = Count - 1;
        ssize StackPointer = 0; // the number of frames pushed to the stack
        struct
        {
            ssize Left, Right;
        } Stack[sizeof(ssize) > 4 && sizeof(Count) > 4 ? 48 : 32];
        while (true)
        {
            if (Right - Left + 1 >= Threshold)
            {
                // Partition
                ssize Middle = Left + ((Right - Left) >> 1);

                // Sort 3 elements
                if (LessProc(UserPtr, Middle, Left + 1))
                {
                    if (LessProc(UserPtr, Right, Middle))
                    {
                        SwapProc(UserPtr, Left + 1, Right);
                    }
                    else
                    {
                        SwapProc(UserPtr, Left + 1, Middle);
                        if (LessProc(UserPtr, Right, Middle))
                        {
                            SwapProc(UserPtr, Middle, Right);
                        }
                    }
                }
                else if (LessProc(UserPtr, Right, Middle))
                {
                    SwapProc(UserPtr, Middle, Right);
                    if (LessProc(UserPtr, Middle, Left + 1))
                    {
                        SwapProc(UserPtr, Left + 1, Middle);
                    }
                }

                // Place the median at the beginning
                SwapProc(UserPtr, Left, Middle);

                ssize I = Left + 1;
                ssize J = Right;
                while (true)
                {
                    do
                    {
                        ++I;
                    } while (LessProc(UserPtr, I, Left));
                    do
                    {
                        --J;
                    } while (LessProc(UserPtr, Left, J));
                    if (I >= J)
                    {
                        break;
                    }

                    SwapProc(UserPtr, I, J);
                }

                // Compensate for the I==J case.
                I = J + 1;
                SwapProc(UserPtr, Left, J);
                // The median is not part of the left subfile.
                --J;

                ssize L1, R1, L2, R2;
                if (J - Left >= Right - I)
                {
                    L1 = Left;
                    R1 = J;
                    L2 = I;
                    R2 = Right;
                }
                else
                {
                    L1 = I;
                    R1 = Right;
                    L2 = Left;
                    R2 = J;
                }

                if (L2 == R2)
                {
                    Left = L1;
                    Right = R1;
                }
                else
                {
                    Stack[StackPointer].Left = L1;
                    Stack[StackPointer].Right = R1;
                    StackPointer++;
                    // Process the smaller subfile on the next iteration.
                    Left = L2;
                    Right = R2;
                }
            }
            else
            {
                // Insertion sort
                for (ssize I = Left + 1; I <= Right; ++I)
                {
                    for (ssize J = I; J > Left && LessProc(UserPtr, J, J - 1); --J)
                    {
                        SwapProc(UserPtr, J, J - 1);
                    }
                }

                if (StackPointer == 0)
                {
                    break;
                }

                --StackPointer;
                Left = Stack[StackPointer].Left;
                Right = Stack[StackPointer].Right;
            }
        }
    }
}

namespace mtb::arena
{
    void InternalInsertNextBucket(tBucket*& CurrentBucket, tBucket* NewBucket)
    {
        if(CurrentBucket)
        {
            NewBucket->Prev = CurrentBucket;
            NewBucket->Next = CurrentBucket->Next;
            NewBucket->Next->Prev = NewBucket->Prev->Next = NewBucket;
        }
        else
        {
            NewBucket->Prev = NewBucket->Next = NewBucket;
        }
        CurrentBucket = NewBucket;
    }

    tBucket* InternalUnlinkBucket(tBucket*& CurrentBucket)
    {
        MTB_ASSERT(CurrentBucket != nullptr);

        tBucket* Result = CurrentBucket;
        if(CurrentBucket == CurrentBucket->Prev)
        {
            CurrentBucket = nullptr;
        }
        else
        {
            CurrentBucket->Prev->Next = CurrentBucket->Next;
            CurrentBucket->Next->Prev = CurrentBucket->Prev;
            CurrentBucket = CurrentBucket->Prev;
        }
        Result->Prev = Result->Next = nullptr;
        return Result;
    }

    u8* InternalBucketAlloc(tBucket* Bucket, usize* inout_Size, usize Alignment)
    {
        u8* Result = nullptr;
        if (Bucket && inout_Size)
        {
            Result = Bucket->Data + Bucket->UsedSize;
            usize EffectiveSize = *inout_Size;
            AlignAllocation((void**)&Result, &EffectiveSize, Alignment);
            if (BucketUsedSize(Bucket) + EffectiveSize <= BucketTotalSize(Bucket))
            {
                *inout_Size = EffectiveSize;
            }
            else
            {
                Result = nullptr;
            }
        }
        return Result;
    }

    void* InternalArenaAlloc(tArena& Arena, usize Size, usize Alignment)
    {
        usize EffectiveSize = Size;
        u8* Result = InternalBucketAlloc(Arena.CurrentBucket, &EffectiveSize, Alignment);
        if (!Result)
        {
            Result = InternalBucketAlloc(Arena.FirstFreeBucket, &EffectiveSize, Alignment);
            if (Result)
            {
                InternalInsertNextBucket(Arena.CurrentBucket, InternalUnlinkBucket(Arena.FirstFreeBucket));
            }
            else
            {
                Grow(Arena, Size + Alignment);
                MTB_ASSERT(Arena.CurrentBucket);

                Result = Arena.CurrentBucket->Data;
                AlignAllocation((void**)&Result, &EffectiveSize, Alignment);
                MTB_ASSERT(BucketUsedSize(Arena.CurrentBucket) + EffectiveSize <= BucketTotalSize(Arena.CurrentBucket));
            }
        }

        Arena.CurrentBucket->UsedSize += EffectiveSize;

        return Result;
    }

    tSlice<void> InternalArenaAllocatorProc(void* User, tSlice<void> OldMem, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero)
    {
        tArena* Arena = (tArena*)User;
        MTB_ASSERT(Arena);
        void* NewPtr = ReallocRaw(*Arena, OldMem.Ptr, OldMem.Count, OldAlignment, NewSize, NewAlignment, bClearToZero);
        tSlice<void> Result = PtrSlice(NewPtr, NewSize);
        return Result;
    }
} // namespace mtb::arena

mtb::usize mtb::arena::BucketTotalSize(tBucket* Bucket)
{
    return Bucket ? Bucket->TotalSize : 0;
}

mtb::usize mtb::arena::BucketUsedSize(tBucket* Bucket)
{
    return Bucket ? Bucket->UsedSize : 0;
}

void mtb::arena::Grow(tArena& Arena, usize RequiredSize)
{
    if(Arena.MinBucketSize == 0)
    {
        Arena.MinBucketSize = MTB_ARENA_DEFAULT_BUCKET_SIZE;
    }

    usize NewBucketSize = BucketTotalSize(Arena.CurrentBucket);
    if(NewBucketSize < Arena.MinBucketSize)
    {
        NewBucketSize = Arena.MinBucketSize;
    }
    while(NewBucketSize < RequiredSize)
    {
        NewBucketSize *= 2;
    }

    tAllocator Allocator = GetChildAllocator(Arena);
    if(Allocator)
    {
        tBucket* NewBucket = (tBucket*)AllocRaw(Allocator, sizeof(tBucket) - sizeof(u8) + NewBucketSize, alignof(tBucket)).Ptr;
        // #TODO Handle out-of-memory properly.
        MTB_ASSERT(NewBucket != nullptr);
        NewBucket->UsedSize = 0;
        NewBucket->TotalSize = NewBucketSize;
        InternalInsertNextBucket(Arena.CurrentBucket, NewBucket);

        if(Arena.LargestBucketSize < NewBucketSize)
        {
            Arena.LargestBucketSize = NewBucketSize;
        }
    }
}

mtb::tAllocator mtb::arena::GetChildAllocator(tArena& Arena)
{
    if(StructIsZero(Arena.ChildAllocator))
    {
        Arena.ChildAllocator = GetDefaultAllocator();
    }

    return Arena.ChildAllocator;
}

void mtb::arena::Reserve(tArena& Arena, usize TotalSize)
{
    if (TotalSize > BucketTotalSize(Arena.CurrentBucket))
    {
        Grow(Arena, TotalSize);
    }
}

void mtb::arena::Clear(tArena& Arena, bool bReleaseMemory /*= true*/)
{
    ResetToMarker(Arena, {}, bReleaseMemory);
    if(Arena.FirstFreeBucket && bReleaseMemory)
    {
        tAllocator Allocator = GetChildAllocator(Arena);
        if(Allocator)
        {
            while(Arena.FirstFreeBucket)
            {
                tBucket* Bucket = InternalUnlinkBucket(Arena.FirstFreeBucket);
                DeallocOne(Allocator, Bucket);
            }
        }
    }
}

void* mtb::arena::ReallocRaw(tArena& Arena, void* OldPtr, usize OldSize, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero)
{
    if(OldPtr || OldSize)
    {
        MTB_ASSERT(OldPtr && OldSize);
    }

    ssize DeltaSize = (ssize)NewSize - (ssize)OldSize;
    if(DeltaSize < 0)
    {
        DeltaSize = -DeltaSize;
    }

    void* Result = nullptr;
    if(!OldPtr)
    {
        if(NewSize)
        {
            Result = InternalArenaAlloc(Arena, NewSize, NewAlignment);
            if(bClearToZero)
            {
                MTB_memset(Result, 0, NewSize);
            }
        }
    }
    else
    {
        MTB_ASSERT(OldAlignment == NewAlignment && "Old and new alignment must be the same for now.");

        if(NewSize < OldSize)
        {
            if((uptr)OldPtr + (uptr)OldSize == (uptr)GetMarker(Arena).Ptr())
            {
                // Shrink the existing allocation.
                MTB_ASSERT(Arena.CurrentBucket);
                MTB_ASSERT(Arena.CurrentBucket->UsedSize >= (usize)DeltaSize);
                Arena.CurrentBucket->UsedSize -= DeltaSize;
            }

            if(NewSize)
            {
                Result = OldPtr;
            }
        }
        else if(NewSize > OldSize)
        {
            // Can we grow the existing allocation?
            if((uptr)OldPtr + (uptr)OldSize == (uptr)GetMarker(Arena).Ptr() &&
               BucketUsedSize(Arena.CurrentBucket) + DeltaSize <= BucketTotalSize(Arena.CurrentBucket))
            {
                void* NewPtr = InternalArenaAlloc(Arena, DeltaSize, 1);
                MTB_ASSERT((uptr)NewPtr == (uptr)OldPtr + (uptr)OldSize);
                Result = OldPtr;
            }
            else
            {
                Result = InternalArenaAlloc(Arena, NewSize, NewAlignment);
                MTB_ASSERT(Result != nullptr);
                MTB_memcpy(Result, OldPtr, OldSize);
            }

            if(bClearToZero)
            {
                MTB_memset(PtrOffset(Result, OldSize), 0, DeltaSize);
            }
        }
        else
        {
            Result = OldPtr;
        }
    }

    return Result;
}

void* mtb::arena::PushRaw(tArena& Arena, usize Size, usize Alignment, bool bClearToZero)
{
    return ReallocRaw(Arena, nullptr, 0, 0, Size, Alignment, bClearToZero);
}

mtb::tSlice<void> mtb::arena::ReallocRawArray(tArena& Arena, tSlice<void> OldArray, usize OldAlignment, usize NewSize, usize NewAlignment, bool bClearToZero /*= true*/)
{
    void* Ptr = ReallocRaw(Arena, OldArray.Ptr, OldArray.Count, OldAlignment, NewSize, NewAlignment, bClearToZero);
    return PtrSlice(Ptr, NewSize);
}

mtb::tSlice<void> mtb::arena::PushRawArray(tArena& Arena, usize Size, usize Alignment, bool bClearToZero /*= true*/)
{
    return ReallocRawArray(Arena, {}, 0, Size, Alignment, bClearToZero);
}

mtb::arena::tArenaMarker mtb::arena::GetMarker(tArena& Arena)
{
    tArenaMarker Result{Arena.CurrentBucket, BucketUsedSize(Arena.CurrentBucket)};
    return Result;
}

void mtb::arena::ResetToMarker(tArena& Arena, tArenaMarker Marker, bool bReleaseMemory /*= true*/)
{
    if (Arena.CurrentBucket)
    {
        tAllocator Allocator = GetChildAllocator(Arena);
        if(!Allocator)
        {
            bReleaseMemory = false;
        }

        bool bWasNull = Marker.Bucket == nullptr;
        if(!Marker.Bucket)
        {
            // Treat the empty marker as a marker of the oldest bucket.
            Marker.Bucket = Arena.CurrentBucket->Next;
        }

        // search backwards through all buckets, freeing them if they're not
        // the one we're looking for.
        bool bLooping = true;
        while(bLooping)
        {
            if(Arena.CurrentBucket == Marker.Bucket)
            {
                bLooping = false;
                if(Marker.Offset > 0)
                {
                    break;
                }
            }

            tBucket* FreeBucket = InternalUnlinkBucket(Arena.CurrentBucket);
            if(bReleaseMemory)
            {
                DeallocRaw(Allocator, PtrSlice((void*)FreeBucket, sizeof(tBucket) - sizeof(u8) + FreeBucket->TotalSize), alignof(tBucket));
            }
            else
            {
                InternalInsertNextBucket(Arena.FirstFreeBucket, FreeBucket);
            }
        }

        if(Arena.CurrentBucket)
        {
            Arena.CurrentBucket->UsedSize = Marker.Offset;
        }
    }
}

mtb::tSlice<void> mtb::arena::Linearize(tArena& Arena, tArenaMarker Begin, tArenaMarker End)
{
    if(!End.Bucket)
    {
        MTB_ASSERT(!Begin.Bucket);
    }
    else if(!Begin.Bucket)
    {
        MTB_ASSERT(Arena.CurrentBucket);
        // If the begin marker has a null-bucket, use the first/oldest bucket.
        Begin.Bucket = Arena.CurrentBucket->Next;
    }

    tSlice<void> Result;
    if(Begin.Bucket == End.Bucket)
    {
        MTB_ASSERT(Begin.Offset <= End.Offset);
        Result = PtrSliceBetween(Begin.Ptr(), End.Ptr());
    }
    else
    {
        MTB_ASSERT(End.Bucket);

        // determine the size
        usize RequiredSize = Begin.Bucket->UsedSize - Begin.Offset;
        for(tBucket* Bucket = Begin.Bucket->Next; Bucket != End.Bucket; Bucket = Bucket->Next)
        {
            RequiredSize += Bucket->UsedSize;
        }
        RequiredSize += End.Offset;

        // allocate data
        Result = PushArray<u8>(Arena, RequiredSize, eArenaInit::kNoInit);

        // copy the data
        usize Cursor = 0;
        MTB_memcpy(Result + Cursor, Begin.Bucket->Data + Begin.Offset, Begin.Bucket->UsedSize - Begin.Offset);
        Cursor += Begin.Bucket->UsedSize - Begin.Offset;
        for(tBucket* Bucket = Begin.Bucket->Next; Bucket != End.Bucket; Bucket = Bucket->Next)
        {
            MTB_memcpy(Result + Cursor, Bucket->Data, Bucket->UsedSize);
            Cursor += Bucket->UsedSize;
        }
        MTB_memcpy(Result + Cursor, End.Bucket->Data, End.Offset);
    }

    return Result;
}

mtb::tAllocator mtb::arena::MakeAllocator(tArena& Arena)
{
    tAllocator Result{};
    Result.User = &Arena;
    Result.ReallocProc = InternalArenaAllocatorProc;
    return Result;
}

#if MTB_USE_STB_SPRINTF
namespace mtb::arena
{
    static char* InternalArenaPrintCallback(char const* buf, void* user, int len)
    {
        tArena* Arena = (tArena*)user;
        tSlice<void> Dest = PushRawArray(*Arena, len, 1, false);
        SliceCopyRaw(Dest, PtrSlice(buf, len));
        return (char*)buf;
    }
} // namespace mtb::arena

void mtb::arena::vprintf_ArenaRaw(tArena& Arena, char const* Format, va_list VArgs)
{
    char TempBuffer[STB_SPRINTF_MIN];
    stbsp_vsprintfcb(InternalArenaPrintCallback, &Arena, TempBuffer, Format, VArgs);
}

mtb::tSlice<char> mtb::arena::vprintf_Arena(tArena& Arena, char const* Format, va_list VArgs)
{
    // Remember where we were in this arena.
    tArenaMarker BeginMarker = GetMarker(Arena);

    // Produce formatted string fragments within the arena.
    vprintf_ArenaRaw(Arena, Format, VArgs);

    // Append the null-terminator.
    *PushOne<char>(Arena, eArenaInit::kNoInit) = 0;

    // Make sure all formatted string fragments we produced are actually linear in memory.
    tSlice<char> Result = SliceCast<char>(Linearize(Arena, BeginMarker, GetMarker(Arena)));
    MTB_ASSERT(Result.Count > 0);

    // Remove the null-terminator from the result.
    Result.Count--;

    return Result;
}
#endif // MTB_USE_STB_SPRINTF

namespace mtb::string
{
    template<typename C>
    C Impl_ToLowerChar(C Char)
    {
        switch (Char)
        {
            case 'A': return 'a';
            case 'B': return 'b';
            case 'C': return 'c';
            case 'D': return 'd';
            case 'E': return 'e';
            case 'F': return 'f';
            case 'G': return 'g';
            case 'H': return 'h';
            case 'I': return 'i';
            case 'J': return 'j';
            case 'K': return 'k';
            case 'L': return 'l';
            case 'M': return 'm';
            case 'N': return 'n';
            case 'O': return 'o';
            case 'P': return 'p';
            case 'Q': return 'q';
            case 'R': return 'r';
            case 'S': return 's';
            case 'T': return 't';
            case 'U': return 'u';
            case 'V': return 'v';
            case 'W': return 'w';
            case 'X': return 'x';
            case 'Y': return 'y';
            case 'Z': return 'z';
            default: return Char;
        }
    }

    template<typename C>
    C Impl_ToUpperChar(C Char)
    {
        switch (Char)
        {
            case 'a': return 'A';
            case 'b': return 'B';
            case 'c': return 'C';
            case 'd': return 'D';
            case 'e': return 'E';
            case 'f': return 'F';
            case 'g': return 'G';
            case 'h': return 'H';
            case 'i': return 'I';
            case 'j': return 'J';
            case 'k': return 'K';
            case 'l': return 'L';
            case 'm': return 'M';
            case 'n': return 'N';
            case 'o': return 'O';
            case 'p': return 'P';
            case 'q': return 'Q';
            case 'r': return 'R';
            case 's': return 'S';
            case 't': return 'T';
            case 'u': return 'U';
            case 'v': return 'V';
            case 'w': return 'W';
            case 'x': return 'X';
            case 'y': return 'Y';
            case 'z': return 'Z';
            default: return Char;
        }
    }

    template<typename C>
    bool Impl_IsDigitChar(C Char)
    {
        switch (Char)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return true;
            default:
                return false;
        }
    }

    template<typename C>
    bool Impl_IsWhiteChar(C Char)
    {
        switch (Char)
        {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
            case '\b':
                return true;
            default:
                return false;
        }
    }

    template<typename C>
    s32 Impl_StringCompare(tSlice<C const> StrA, tSlice<C const> StrB, eStringComparison Cmp)
    {
        s32 result = 0;

        if (StrA.Count != StrB.Count)
        {
            result = IntCast<s32>(StrA.Count - StrB.Count);
        }
        else
        {
            usize Count = StrA.Count;
            switch (Cmp)
            {
                case eStringComparison::kCaseSensitive:
                {
                    for (usize ByteIndex = 0; ByteIndex < Count; ++ByteIndex)
                    {
                        if (StrA.Ptr[ByteIndex] != StrB.Ptr[ByteIndex])
                        {
                            result = StrA.Ptr[ByteIndex] < StrB.Ptr[ByteIndex] ? -1 : 1;
                            break;
                        }
                    }
                }
                break;

                case eStringComparison::kIgnoreCase:
                {
                    for (usize ByteIndex = 0; ByteIndex < Count; ++ByteIndex)
                    {
                        C CharA = ToLowerChar(StrA.Ptr[ByteIndex]);
                        C CharB = ToLowerChar(StrB.Ptr[ByteIndex]);
                        if (CharA != CharB)
                        {
                            result = (s32)(CharA - CharB);
                            break;
                        }
                    }
                }
                break;
            }
        }

        return result;
    }

    template<typename C>
    bool Impl_StringStartsWith(tSlice<C const> Str, tSlice<C const> Prefix, eStringComparison Cmp)
    {
        bool bResult = false;
        if (Str.Count >= Prefix.Count)
        {
            bResult = StringEquals(SliceRange(Str, 0, Prefix.Count), Prefix, Cmp);
        }

        return bResult;
    }

    template<typename C>
    bool Impl_StringEndsWith(tSlice<C const> Str, tSlice<C const> Prefix, eStringComparison Cmp)
    {
        bool bResult = false;
        if (Str.Count >= Prefix.Count)
        {
            usize Offset = Str.Count - Prefix.Count;
            bResult = StringEquals(SliceBetween(Str, Offset, Str.Count), Prefix, Cmp);
        }

        return bResult;
    }

    template<typename C, typename P>
    tSlice<C> Impl_StringTrimStartPredicate(tSlice<C> Str, P Predicate)
    {
        ssize Offset = 0;
        if (Predicate)
        {
            for (; Offset < Str.Count; ++Offset)
            {
                C Char = Str.Ptr[Offset];
                bool bIsValid = Predicate(Char);
                if (!bIsValid)
                {
                    break;
                }
            }
        }

        return SliceBetween(Str, Offset, Str.Count);
    }

    template<typename C, typename P>
    tSlice<C> Impl_StringTrimEndPredicate(tSlice<C> Str, P Predicate)
    {
        ssize OnePastLastIndex = Str.Count;
        if (Predicate)
        {
            while (OnePastLastIndex > 0 && Predicate(Str.Ptr[OnePastLastIndex - 1]))
            {
                --OnePastLastIndex;
            }
        }

        return SliceBetween(Str, 0, OnePastLastIndex);
    }

    template<typename C>
    tSlice<C> Impl_AllocString(tAllocator A, usize CharCount)
    {
        tSlice<C> Result = AllocArray<C>(A, CharCount + 1);
        SliceDefaultConstructItems(SliceOffset(Result, CharCount));
        return SliceRange(Result, 0, Result.Count - 1);
    }

    template<typename CIn, typename COut = tRemoveConst<CIn>>
    tSlice<COut> Impl_AllocStringCopy(tAllocator A, tSlice<CIn> String)
    {
        tSlice<COut> Result = AllocArray<COut>(A, String.Count + 1);
        SliceCopyRaw(Result, String);
        SliceDefaultConstructItems(SliceOffset(Result, String.Count));
        return SliceRange(Result, 0, Result.Count - 1);
    }

    template<typename C>
    usize Impl_StringLengthZ(C const* StrZ)
    {
        usize Result = 0;
        if (StrZ)
        {
            while (StrZ[Result])
            {
                ++Result;
            }
        }

        return Result;
    }
} // namespace mtb::string

char mtb::string::ToLowerChar(char Char)
{
    return Impl_ToLowerChar(Char);
}

char mtb::string::ToUpperChar(char Char)
{
    return Impl_ToUpperChar(Char);
}

bool mtb::string::IsDigitChar(char Char)
{
    return Impl_IsDigitChar(Char);
}

bool mtb::string::IsWhiteChar(char Char)
{
    return Impl_IsWhiteChar(Char);
}

mtb::s32 mtb::string::StringCompare(tSlice<char const> StrA, tSlice<char const> StrB, eStringComparison Cmp /*= eStringComparison::kCaseSensitive*/)
{
    return Impl_StringCompare(StrA, StrB, Cmp);
}

bool mtb::string::StringEquals(tSlice<char const> StrA, tSlice<char const> StrB, eStringComparison Cmp)
{
    return Impl_StringCompare(StrA, StrB, Cmp) == 0;
}

bool mtb::string::StringStartsWith(tSlice<char const> Str, tSlice<char const> Prefix, eStringComparison Cmp)
{
    return Impl_StringStartsWith(Str, Prefix, Cmp);
}

bool mtb::string::StringEndsWith(tSlice<char const> Str, tSlice<char const> Prefix, eStringComparison Cmp)
{
    return Impl_StringEndsWith(Str, Prefix, Cmp);
}

mtb::tSlice<char const> mtb::string::StringTrimStartPredicate(tSlice<char const> Str, tTrimPredicate Predicate)
{
    return Impl_StringTrimStartPredicate<char const>(Str, Predicate);
}

mtb::tSlice<char const> mtb::string::StringTrimEndPredicate(tSlice<char const> Str, tTrimPredicate Predicate)
{
    return Impl_StringTrimEndPredicate(Str, Predicate);
}

mtb::tSlice<char const> mtb::string::StringTrimPredicate(tSlice<char const> Str, tTrimPredicate Predicate)
{
    return StringTrimStartPredicate(StringTrimEndPredicate(Str, Predicate), Predicate);
}

mtb::tSlice<char const> mtb::string::StringTrimStart(tSlice<char const> Str)
{
    return StringTrimStartPredicate(Str, IsWhiteChar);
}

mtb::tSlice<char const> mtb::string::StringTrimEnd(tSlice<char const> Str)
{
    return StringTrimEndPredicate(Str, IsWhiteChar);
}

mtb::tSlice<char const> mtb::string::StringTrim(tSlice<char const> Str)
{
    return StringTrimPredicate(Str, IsWhiteChar);
}

mtb::tSlice<char> mtb::string::AllocString(tAllocator A, usize CharCount)
{
    return Impl_AllocString<char>(A, CharCount);
}

mtb::tSlice<char> mtb::string::AllocStringCopy(tAllocator A, tSlice<char const> String)
{
    return Impl_AllocStringCopy(A, String);
}

void mtb::string::DeallocString(tAllocator A, tSlice<char> String)
{
    ++String.Count;
    DeallocArray(A, String);
}

mtb::usize mtb::string::StringLengthZ(char const* StrZ)
{
    return Impl_StringLengthZ(StrZ);
}

wchar_t mtb::string::ToLowerChar(wchar_t Char)
{
    return Impl_ToLowerChar(Char);
}

wchar_t mtb::string::ToUpperChar(wchar_t Char)
{
    return Impl_ToUpperChar(Char);
}

bool mtb::string::IsDigitChar(wchar_t Char)
{
    return Impl_IsDigitChar(Char);
}

bool mtb::string::IsWhiteChar(wchar_t Char)
{
    return Impl_IsWhiteChar(Char);
}

mtb::s32 mtb::string::StringCompare(tSlice<wchar_t const> StrA, tSlice<wchar_t const> StrB, eStringComparison Cmp /*= eStringComparison::kCaseSensitive*/)
{
    return Impl_StringCompare(StrA, StrB, Cmp);
}

bool mtb::string::StringEquals(tSlice<wchar_t const> StrA, tSlice<wchar_t const> StrB, eStringComparison Cmp)
{
    return Impl_StringCompare(StrA, StrB, Cmp) == 0;
}

bool mtb::string::StringStartsWith(tSlice<wchar_t const> Str, tSlice<wchar_t const> Prefix, eStringComparison Cmp)
{
    return Impl_StringStartsWith(Str, Prefix, Cmp);
}

bool mtb::string::StringEndsWith(tSlice<wchar_t const> Str, tSlice<wchar_t const> Prefix, eStringComparison Cmp)
{
    return Impl_StringEndsWith(Str, Prefix, Cmp);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrimStartPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate)
{
    return Impl_StringTrimStartPredicate(Str, Predicate);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrimEndPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate)
{
    return Impl_StringTrimEndPredicate(Str, Predicate);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrimPredicate(tSlice<wchar_t const> Str, tWTrimPredicate Predicate)
{
    return StringTrimStartPredicate(StringTrimEndPredicate(Str, Predicate), Predicate);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrimStart(tSlice<wchar_t const> Str)
{
    return StringTrimStartPredicate(Str, IsWhiteChar);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrimEnd(tSlice<wchar_t const> Str)
{
    return StringTrimEndPredicate(Str, IsWhiteChar);
}

mtb::tSlice<wchar_t const> mtb::string::StringTrim(tSlice<wchar_t const> Str)
{
    return StringTrimPredicate(Str, IsWhiteChar);
}

mtb::tSlice<wchar_t> mtb::string::AllocWString(tAllocator A, usize CharCount)
{
    return Impl_AllocString<wchar_t>(A, CharCount);
}

mtb::tSlice<wchar_t> mtb::string::AllocStringCopy(tAllocator A, tSlice<wchar_t const> String)
{
    return Impl_AllocStringCopy(A, String);
}

mtb::usize mtb::string::StringLengthZ(wchar_t const* StrZ)
{
    return Impl_StringLengthZ(StrZ);
}

void mtb::string::DeallocWString(tAllocator A, tSlice<wchar_t> String)
{
    ++String.Count;
    DeallocArray(A, String);
}

void mtb::BreakIntoUnits(u64 Value, tSlice<u64 const> UnitsTable, tSlice<u32> out_Results)
{
    u64 Remaining = Value;
    for (ssize Index = 0; Index < UnitsTable.Count; ++Index)
    {
        u64 Unit = UnitsTable.Ptr[Index];
        u32 Result = (u32)(Remaining / Unit);
        Remaining -= (u64)(Result * Unit);

        out_Results[Index] = Result;
    }
}

mtb::time::tClockTime mtb::time::ClockTimeFromTimespan(tTimespan Timespan)
{
    tClockTime Result{};
    u64 Total;
    if (Timespan.Nanoseconds < 0)
    {
        Total = IntCast<u64>(-Timespan.Nanoseconds);
        Result.negative = true;
    }
    else
    {
        Total = IntCast<u64>(Timespan.Nanoseconds);
    }

    u64 Units[7]{
        kDaysToNanoseconds,
        kHoursToNanoseconds,
        kMinutesToNanoseconds,
        kSecondsToNanoseconds,
        kMillisecondsToNanoseconds,
        kMicrosecondsToNanoseconds,
        1,
    };
    BreakIntoUnits(Total, ArraySlice(Units), PtrSlice(&Result.days, 7));

    return Result;
}

//
// #Section Tests
//
#if MTB_TESTS

namespace mtb
{
    static doctest::String toString(mtb::tSlice<char const> StringSlice)
    {
        // #NOTE Doctest has kind of a weird string API. It always expects
        // null-terminated strings and uses strcmp but it provides a
        // constructor that takes in a length. It tries to copy the trailing
        // null-terminator by looking one past the size we pass in there,
        // which is illegal for non-null-terminated strings since THERE'S NO
        // NULL TERMINATOR THERE. Anyway, be aware that we're tricking the
        // doctest String class here so this code might break in future
        // doctest releases.
        if(StringSlice.Count == 0)
        {
            return "";
        }

        doctest::String Result{(char const*)StringSlice.Ptr, (unsigned)StringSlice.Count - 1};
        char Trick[]{*SliceLast(StringSlice), 0};
        Result += Trick;
        return Result;
    }
}

DOCTEST_TEST_SUITE("mtb slices")
{
    using namespace mtb;

    DOCTEST_TEST_CASE("SliceCast")
    {
        char Data[]{'a', 'b', 'c', 'd', 'e', 'f'};
        tSlice<char> Chars = ArraySlice(Data);
        CHECK(Chars.Count == 6);

        tSlice<u16> Shorts = SliceCast<u16>(Chars);
        CHECK(Shorts.Count == 3);

        tSlice<void> Void = Shorts;
        CHECK(Void.Count == 6);
        u8 RefByte = (u8)'a';
        for(u8 Byte : SliceCast<u8>(Void))
        {
            CHECK(Byte == RefByte++);
        }
    }
}

DOCTEST_TEST_SUITE("mtb arena")
{
    using namespace mtb;
    using namespace mtb::arena;

    ssize BucketCount(tArena& Arena)
    {
        ssize Result = 0;
        if(Arena.CurrentBucket)
        {
            Result = 1;
            for(tBucket* Bucket = Arena.CurrentBucket->Next; Bucket != Arena.CurrentBucket; Bucket = Bucket->Next)
            {
                ++Result;
            }
        }
        return Result;
    }

    DOCTEST_TEST_CASE("General")
    {
        tArena Arena{};
        Arena.MinBucketSize = 1024;
        DOCTEST_CHECK(BucketCount(Arena) == 0);
        u8* P0 = (u8*)PushRaw(Arena, 1023, 1, false);
        DOCTEST_CHECK(BucketCount(Arena) == 1);
        memset(P0, 2, 1023);
        u8* P1 = (u8*)PushRaw(Arena, 1, 1024, false);
        DOCTEST_CHECK(BucketCount(Arena) == 2);
        DOCTEST_CHECK((uintptr_t)P1 % 1024 == 0);
        *P1 = 3;
        u8* P2 = (u8*)PushRaw(Arena, 1, 1024, false);
        DOCTEST_CHECK(BucketCount(Arena) == 2);
        DOCTEST_CHECK((uintptr_t)P2 % 1024 == 0);
        *P2 = 4;

        for (int Index = 0; Index < 1023; ++Index)
        {
            DOCTEST_CHECK(P0[Index] == 2);
        }
        DOCTEST_CHECK(*P1 == 3);
        DOCTEST_CHECK(*P2 == 4);

        Clear(Arena, true);
        DOCTEST_CHECK(BucketCount(Arena) == 0);
    }

    DOCTEST_TEST_CASE("Marker")
    {
        tArena Arena{};
        Arena.MinBucketSize = 1024;
        DOCTEST_CHECK(BucketUsedSize(Arena.CurrentBucket) == 0);

        tArenaMarker ZeroMarker{};

        PushRaw(Arena, 512, 1, false);
        DOCTEST_CHECK(BucketUsedSize(Arena.CurrentBucket) == 512);

        DOCTEST_SUBCASE("force bucket overflow")
        {
            tArenaMarker Marker = GetMarker(Arena);

            PushRaw(Arena, 1024, 1, false);
            DOCTEST_CHECK(Arena.CurrentBucket != Marker.Bucket);

            ResetToMarker(Arena, Marker, false);
            DOCTEST_CHECK(Arena.CurrentBucket == Marker.Bucket);
        }

        DOCTEST_SUBCASE("free entire arena via marker")
        {
            ResetToMarker(Arena, ZeroMarker, false);
            DOCTEST_CHECK(Arena.CurrentBucket == nullptr);
            DOCTEST_CHECK(BucketUsedSize(Arena.CurrentBucket) == 0);
        }

        DOCTEST_SUBCASE("linearize")
        {
            tArenaMarker Marker0 = GetMarker(Arena);

            void* P0 = PushRaw(Arena, 1024, 1, false);
            void* P1 = PushRaw(Arena, 1024, 1, false);
            DOCTEST_CHECK(P0 != P1);

            tSlice<void> Linear = Linearize(Arena, Marker0, GetMarker(Arena));
            tArenaMarker Marker1 = GetMarker(Arena);
            DOCTEST_CHECK(Linear.Ptr != P0);
            DOCTEST_CHECK(Linear.Ptr != P1);
            DOCTEST_CHECK(BucketUsedSize(Arena.CurrentBucket) >= 2 * 1024);
            DOCTEST_CHECK(Marker1.Bucket != nullptr);
            DOCTEST_CHECK(BucketTotalSize(Marker1.Bucket) >= 2 * 1024);
        }

        Clear(Arena, true);
    }

    DOCTEST_TEST_CASE("Linearize")
    {
        tArena Arena{};
        Arena.MinBucketSize = 1;
        MTB_DEFER { Clear(Arena, true); };

        *PushOne<int>(Arena) = 1;
        *PushOne<int>(Arena) = 2;
        *PushOne<int>(Arena) = 3;
        tArenaMarker MidMarker = GetMarker(Arena);
        *PushOne<int>(Arena) = 4;
        *PushOne<int>(Arena) = 5;
        *PushOne<int>(Arena) = 6;
        *PushOne<int>(Arena) = 7;
        tArenaMarker EndMarker = GetMarker(Arena);

        tSlice<int> Left = SliceCast<int>(Linearize(Arena, {}, MidMarker));
        DOCTEST_CHECK(Left.Count == 3);
        DOCTEST_CHECK(Left[0] == 1);
        DOCTEST_CHECK(Left[1] == 2);
        DOCTEST_CHECK(Left[2] == 3);

        tSlice<int> Right = SliceCast<int>(Linearize(Arena, MidMarker, EndMarker));
        DOCTEST_CHECK(Right.Count == 4);
        DOCTEST_CHECK(Right[0] == 4);
        DOCTEST_CHECK(Right[1] == 5);
        DOCTEST_CHECK(Right[2] == 6);
        DOCTEST_CHECK(Right[3] == 7);

        tSlice<int> All = SliceCast<int>(Linearize(Arena, {}, EndMarker));
        DOCTEST_CHECK(All.Count == 7);
        DOCTEST_CHECK(All[0] == 1);
        DOCTEST_CHECK(All[1] == 2);
        DOCTEST_CHECK(All[2] == 3);
        DOCTEST_CHECK(All[3] == 4);
        DOCTEST_CHECK(All[4] == 5);
        DOCTEST_CHECK(All[5] == 6);
        DOCTEST_CHECK(All[6] == 7);
    }

#if MTB_USE_STB_SPRINTF
    DOCTEST_TEST_CASE("printf")
    {
        tArena Arena{};
        MTB_DEFER { Clear(Arena, true); };

        tSlice<char> Str = printf_Arena(Arena, "Hello world!");
        DOCTEST_CHECK(toString(Str) == "Hello world!");

        Clear(Arena, false);

        DOCTEST_SUBCASE("manual linearization")
        {
            // This produces a formatted non-null-terminated string.
            Arena.MinBucketSize = 1;
            tArenaMarker BeginMarker = GetMarker(Arena);
            printf_ArenaRaw(Arena, "%c", 'a');
            printf_ArenaRaw(Arena, "%c", 'b');
            printf_ArenaRaw(Arena, "%c", 'c');
            printf_ArenaRaw(Arena, "%c", 'd');
            printf_ArenaRaw(Arena, "%c", 'e');
            printf_ArenaRaw(Arena, "%c", 'f');
            tArenaMarker EndMarker = GetMarker(Arena);
            tSlice<char> Linearized = SliceCast<char>(Linearize(Arena, BeginMarker, EndMarker));
            DOCTEST_CHECK(toString(Linearized) == "abcdef");
        }

        DOCTEST_SUBCASE("force linearization over multiple buckets")
        {
            Arena.MinBucketSize = 8;

            char const* lipsum =
                "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed cursus "
                "hendrerit pharetra. Cras sit amet blandit arcu."
                "Mauris tristique dapibus vulputate. Maecenas ipsum dui, porta id "
                "felis quis, gravida dictum neque. Suspendisse id"
                "fringilla justo. Duis volutpat lobortis magna et imperdiet. Etiam "
                "vulputate nisl eu lacus pulvinar sodales. Mauris eu"
                "diam nulla. Pellentesque commodo, ligula in varius eleifend, velit "
                "metus fermentum sapien, a accumsan nisl metus vel"
                "lorem. Aliquam ultricies nisl sed ex tincidunt semper. Ut ut turpis "
                "eleifend, sagittis velit eu, lobortis elit. Mauris"
                "pellentesque dignissim elit, quis suscipit ante iaculis vitae. "
                "Aenean non gravida ex. Mauris lobortis mi massa, id"
                "bibendum eros laoreet lobortis. Fusce nisi libero, dictum et dui "
                "iaculis, pellentesque aliquet erat. Cras pharetra metus"
                "nec sapien consectetur, vitae viverra sem sollicitudin. Fusce id "
                "condimentum leo. Ut gravida sed nisi eu convallis."
                "Nulla volutpat metus id nisi ultricies, nec gravida libero laoreet. "
                "Vestibulum bibendum leo lorem, vitae maximus leo"
                "luctus non. Pellentesque nec pretium risus. Integer libero neque, "
                "molestie vel erat sit amet, euismod rutrum enim."
                "Maecenas tincidunt magna sapien, vel iaculis est vehicula a. "
                "Suspendisse sollicitudin purus quis magna interdum, id"
                "bibendum tortor sollicitudin. Phasellus eget lectus feugiat, luctus "
                "diam et, venenatis felis. Cras sit amet tellus non"
                "est aliquam posuere. Curabitur eu aliquet arcu. Suspendisse a "
                "sollicitudin eros. Donec ac egestas ante. Pellentesque id"
                "pellentesque turpis. Pellentesque lobortis vel elit fringilla "
                "scelerisque. Donec eu posuere lacus, vitae iaculis metus."
                "Aliquam eleifend facilisis quam, et fringilla nisl ornare "
                "facilisis. Phasellus in massa ac risus posuere maximus."
                "Vestibulum id velit id quam eleifend fermentum. Interdum et "
                "malesuada fames ac ante ipsum primis in faucibus. Integer eu"
                "rutrum est. Curabitur non risus non felis dictum scelerisque. "
                "Maecenas id rutrum dolor, in sagittis mauris. Donec id"
                "nunc vel nisl volutpat vulputate nec sed leo. Suspendisse potenti. "
                "Integer cursus, ante eu ullamcorper imperdiet, orci"
                "metus porta ligula, at mollis est urna quis massa. Mauris faucibus "
                "placerat purus in volutpat. Etiam lacinia non tellus"
                "at iaculis. Maecenas bibendum odio volutpat varius fermentum. Sed "
                "libero massa, semper eget scelerisque a, sodales vel"
                "felis. Integer sodales augue eu felis hendrerit, ac malesuada felis "
                "auctor. In hac habitasse platea dictumst. Phasellus"
                "maximus eu magna id pellentesque. Fusce quis tempus lacus. "
                "Suspendisse luctus suscipit lacus euismod viverra. Morbi"
                "consequat nec felis id blandit. Morbi nec felis turpis. Cras ornare "
                "enim leo, sed convallis lectus elementum vel. Cras"
                "luctus augue finibus nibh varius consectetur. Quisque elementum, "
                "velit quis egestas tempus, sapien magna faucibus odio,"
                "ac tincidunt odio mauris in erat.";

            tSlice<char> Result = printf_Arena(Arena, "%s", lipsum);
            DOCTEST_CHECK(toString(Result) == lipsum);
        }
    }
#endif // MTB_USE_STB_SPRINTF
}

#endif // MTB_TESTS
#endif // MTB_IMPLEMENTATION

/*
<!-- BEGIN NATVIS -->

<?xml version="1.0" encoding="utf-8"?>
<AutoVisualizer xmlns="http://schemas.microsoft.com/vstudio/debugger/natvis/2010">
    <Type Name="::mtb::arena::tBucket">
        <DisplayString>Used={UsedSize}/{TotalSize}</DisplayString>
        <Expand>
            <Item Name="Used Size">UsedSize</Item>
            <Item Name="Unused Size">TotalSize - UsedSize</Item>
            <Item Name="Total Size">TotalSize</Item>
            <Item Name="Next Bucket">Next</Item>
            <Item Name="Prev Bucket">Prev</Item>
            <Synthetic Name="[Used Data]">
                <Expand>
                    <ArrayItems>
                        <Size>UsedSize</Size>
                        <ValuePointer>Data</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <Synthetic Name="[Unused Data]">
                <Expand>
                    <ArrayItems>
                        <Size>TotalSize - UsedSize</Size>
                        <ValuePointer>Data + UsedSize</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <Synthetic Name="[All Data]">
                <Expand>
                    <ArrayItems>
                        <Size>TotalSize</Size>
                        <ValuePointer>Data</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
        </Expand>
    </Type>

    <Type Name="::mtb::arena::tArena">
        <DisplayString>Current Bucket {*CurrentBucket}</DisplayString>
    </Type>

    <Type Name="::mtb::tSlice &lt; * &gt;">
        <DisplayString>{{{Count} items ({Count * sizeof(*Ptr)} bytes)}}</DisplayString>
        <Expand>
            <ExpandedItem>Ptr,[Count]</ExpandedItem>
        </Expand>
    </Type>

    <!-- string slices -->
    <Type Name="::mtb::tSlice &lt; char &gt;">
        <AlternativeType Name="::mtb::tSlice &lt; char const &gt;"/>
        <AlternativeType Name="::mtb::tSlice &lt; const char &gt;"/>
        <DisplayString>({Count}) {Ptr,[Count]s8}</DisplayString>
        <StringView>Ptr,[Count]s8</StringView>
        <Expand>
            <Item Name="Is zero-terminated">Ptr[Count] == 0</Item>
            <Item Name="Count">Count</Item>
            <Item Name="Data">Ptr,[Count]</Item>
        </Expand>
    </Type>

    <Type Name="::mtb::tSlice &lt; wchar_t &gt;">
        <AlternativeType Name="::mtb::tSlice &lt; wchar_t const &gt;"/>
        <AlternativeType Name="::mtb::tSlice &lt; const wchar_t &gt;"/>
        <DisplayString>({Count}) {Ptr,[Count]su}</DisplayString>
        <StringView>Ptr,[Count]su</StringView>
        <Expand>
            <Item Name="Is zero-terminated">Ptr[Count] == 0</Item>
            <Item Name="Count">Count</Item>
            <Item Name="Data">Ptr,[Count]</Item>
        </Expand>
    </Type>

    <!-- void slices -->
    <Type Name="::mtb::tSlice &lt; void &gt;">
        <AlternativeType Name="::mtb::tSlice &lt; void const &gt;"/>
        <AlternativeType Name="::mtb::tSlice &lt; const void &gt;"/>
        <DisplayString>{Count} bytes</DisplayString>
        <Expand>
            <ExpandedItem>(char unsigned*)Ptr,[Count]</ExpandedItem>
        </Expand>
    </Type>

    <Type Name="::mtb::tArray &lt; * &gt;">
        <DisplayString>{Slice}</DisplayString>
        <Expand>
            <Synthetic Name="[Slack]">
                <DisplayString>{{{Capacity - Count} items ({(Capacity - Count) * sizeof(*Ptr)} bytes)}}</DisplayString>
                <Expand>
                    <ArrayItems>
                        <Size>Capacity - Count</Size>
                        <ValuePointer>Ptr + Count</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <Synthetic Name="[Allocation]">
                <DisplayString>{{{Capacity} items ({Capacity * sizeof(*Ptr)} bytes)}}</DisplayString>
                <Expand>
                    <ArrayItems>
                        <Size>Capacity</Size>
                        <ValuePointer>Ptr</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <ExpandedItem>Slice</ExpandedItem>
        </Expand>
    </Type>

    <Type Name="::mtb::tMap &lt; * &gt;">
        <DisplayString>Count={Count}</DisplayString>
        <Expand>
            <Item Name="Capacity">Capacity</Item>
            <Synthetic Name="Fill %">
                <DisplayString>{(100.0 * (double)Count / (double)Capacity),f}%</DisplayString>
            </Synthetic>
            <Synthetic Name="[Slots]">
                <Expand>
                    <ArrayItems>
                        <Size>Capacity</Size>
                        <ValuePointer>Slots</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <Synthetic Name="[Keys]">
                <Expand>
                    <ArrayItems>
                        <Size>Capacity</Size>
                        <ValuePointer>Keys</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <Synthetic Name="[Values]">
                <Expand>
                    <ArrayItems>
                        <Size>Capacity</Size>
                        <ValuePointer>Values</ValuePointer>
                    </ArrayItems>
                </Expand>
            </Synthetic>
            <CustomListItems MaxItemsPerView="512">
                <Variable Name="Index" InitialValue="0"/>
                <Loop Condition="Index &lt; Capacity">
                    <If Condition="Slots[Index].State == 1">
                        <Item Name="{Keys[Index]}">Values[Index]</Item>
                    </If>
                    <Exec>++Index</Exec>
                </Loop>
            </CustomListItems>
        </Expand>
    </Type>

    <Type Name="::mtb::tTimespan">
        <DisplayString>{Nanoseconds / (1000.0*1000.0*1000.0)} s</DisplayString>
        <Expand>
            <Item Name="Seconds">Nanoseconds / (1000.0*1000.0*1000.0)</Item>
            <Item Name="Milliseconds">Nanoseconds / (1000.0*1000.0)</Item>
            <Item Name="Nanoseconds">Nanoseconds</Item>
        </Expand>
    </Type>

</AutoVisualizer>

<!-- END NATVIS -->
*/
