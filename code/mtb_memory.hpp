#if !defined(MTB_HEADER_memory)
#define MTB_HEADER_memory

#if defined(MTB_MEMORY_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#if defined(MTB_MEMORY_IMPLEMENTATION) && !defined(MTB_ASSERT_IMPLEMENTATION)
  #define MTB_ASSERT_IMPLEMENTATION
#endif

#include "mtb_common.hpp"
#include "mtb_assert.hpp"

namespace mtb
{
/// \defgroup Memory manipulation functions
///
/// Provides functions to work on chunks of memory.
///
/// Unlike C standard functions such as memcpy and memset, these functions
/// respect the type of the input objects. Refer to the table below to find
/// which C standard functionality is covered by which of the functions
/// defined here.
///
/// C Standard Function | Untyped/Bytes                           | Typed
/// ------------------- | --------------------------------------- | -----
/// memcopy, memmove    | MemCopyBytes                        | MemCopy, MemCopyConstruct, MemMove, MemMoveConstruct
/// memset              | MemSetBytes                         | MemSet, MemConstruct
/// memcmp              | MemCompareBytes, MemEqualBytes  | -
///
///
/// All functions are optimized for POD types.
///
/// @{

/// Copy Size from Source to Destination.
///
/// Destination and Source may overlap.
void
MemCopyBytes(memory_size Size, void* Destination, void const* Source);

/// Fill NumBytes in Destination with the value
void
MemSetBytes(memory_size Size, void* Destination, int Value);

bool
MemEqualBytes(memory_size Size, void const* A, void const* B);

int
MemCompareBytes(memory_size Size, void const* A, void const* B);

bool
MemAreOverlappingBytes(memory_size SizeA, void const* A, memory_size SizeB, void const* B);


/// Calls the constructor of all elements in Destination with Args.
///
/// Args may be empty in which case all elements get default-initialized.
template<typename T, typename... ArgTypes>
void
MemConstruct(size_t Num, T* Destination, ArgTypes&&... Args);

/// Destructs all elements in Destination.
template<typename T>
void
MemDestruct(size_t Num, T* Destination);

/// Copy all elements from Source to Destination.
///
/// Destination and Source may overlap.
template<typename T>
void
MemCopy(size_t Num, T* Destination, T const* Source);

/// Copy all elements from Source to Destination using T's constructor.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
MemCopyConstruct(size_t Num, T* Destination, T const* Source);

/// Move all elements from Source to Destination using T's constructor.
///
/// Destination and Source may overlap.
template<typename T>
void
MemMove(size_t Num, T* Destination, T* Source);

/// Move all elements from Source to Destination using T's constructor and destruct Source afterwards.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
MemMoveConstruct(size_t Num, T* Destination, T* Source);

/// Assign the default value of T to all elements in Destination.
template<typename T>
void
MemSet(size_t Num, T* Destination);

/// Assign Item to all elements in Destination.
template<typename T>
void
MemSet(size_t Num, T* Destination, T const& Item);

template<typename TA, typename TB>
bool
MemAreOverlapping(size_t NumA, TA const* A, size_t NumB, TB const* B)
{
  return MemAreOverlappingBytes(NumA * SizeOf<TA>(), Reinterpret<void const*>(A),
                                NumB * SizeOf<TB>(), Reinterpret<void const*>(B));
}


//
// Implementation Details
//

// MemConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_construct
{
  template<typename... ArgTypes>
  MTB_Inline static void
  Do(size_t Num, T* Destination, ArgTypes&&... Args)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Forward<ArgTypes>(Args)...);
    }
  }
};

template<typename T>
struct impl_mem_construct<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    MemSetBytes(Num * SizeOf<T>(), Destination, 0);
  }

  MTB_Inline static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    // Blit Item over each element of Destination.
    for(size_t Index = 0; Index < Num; ++Index)
    {
      MemCopyBytes(SizeOf<T>(), &Destination[Index], &Item);
    }
  }
};

template<typename T, typename... ArgTypes>
MTB_Inline auto
MemConstruct(size_t Num, T* Destination, ArgTypes&&... Args)
  -> void
{
  impl_mem_construct<T, IsPOD<T>()>::Do(Num, Destination, Forward<ArgTypes>(Args)...);
}


// MemDestruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_destruct
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index].~T();
    }
  }
};

template<typename T>
struct impl_mem_destruct<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    // Nothing to do for POD types.
  }
};

template<typename T>
MTB_Inline auto
MemDestruct(size_t Num, T* Destination)
  -> void
{
  impl_mem_destruct<T, IsPOD<T>()>::Do(Num, Destination);
}


// MemCopy

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_copy
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    if(Destination == Source)
      return;

    if(MemAreOverlapping(Num, Destination, Num, Source) && Destination < Source)
    {
      // Copy backwards.
      for(size_t Index = Num; Index > 0;)
      {
        --Index;
        Destination[Index] = Source[Index];
      }
    }
    else
    {
      // Copy forwards.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = Source[Index];
      }
    }
  }
};

template<typename T>
struct impl_mem_copy<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    MemCopyBytes(SizeOf<T>() * Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
MemCopy(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_mem_copy<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemCopyConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_copy_construct
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_DebugAssert(!MemAreOverlapping(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Source[Index]);
    }
  }
};

template<typename T>
struct impl_mem_copy_construct<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_DebugAssert(!MemAreOverlapping(Num, Destination, Num, Source));

    MemCopy(Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
MemCopyConstruct(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_mem_copy_construct<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemMove

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_move
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    if(Destination == Source)
      return;

    if(MemAreOverlapping(Num, Destination, Num, Source))
    {
      if(Destination < Source)
      {
        // Move forward
        for(size_t Index = 0; Index < Num; ++Index)
        {
          Destination[Index] = Move(Source[Index]);
        }

        // Destroy the remaining elements in the back.
        size_t const NumToDestruct = Source - Destination;
        MemDestruct(NumToDestruct, MemAddOffset(Source, Num - NumToDestruct));
      }
      else
      {
        // Move backward
        for(size_t Index = Num; Index > 0;)
        {
          --Index;
          Destination[Index] = Move(Source[Index]);
        }

        // Destroy the remaining elements in the front.
        size_t const NumToDestruct = Destination - Source;
        MemDestruct(NumToDestruct, Source);
      }
    }
    else
    {
      // Straight forward: Move one by one, then destruct all in Source.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = Move(Source[Index]);
      }
      MemDestruct(Num, Source);
    }
  }
};

template<typename T>
struct impl_mem_move<T, true> : public impl_mem_copy<T, true> {};

template<typename T>
MTB_Inline auto
MemMove(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_mem_move<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemMoveConstruct

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_move_construct
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_DebugAssert(!MemAreOverlapping(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Move(Source[Index]));
    }
    MemDestruct(Num, Source);
  }
};

template<typename T>
struct impl_mem_move_construct<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_DebugAssert(!MemAreOverlapping(Num, Destination, Num, Source));

    MemCopy(Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
MemMoveConstruct(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_mem_move_construct<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MemSet

template<typename T, bool TIsPlainOldData = false>
struct impl_mem_set
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = {};
    }
  }

  MTB_Inline static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Item;
    }
  }
};

template<typename T>
struct impl_mem_set<T, true> : public impl_mem_construct<T, true> {};

template<typename T>
MTB_Inline auto
MemSet(size_t Num, T* Destination)
  -> void
{
  impl_mem_set<T, IsPOD<T>()>::Do(Num, Destination);
}

template<typename T>
MTB_Inline auto
MemSet(size_t Num, T* Destination, T const& Item)
  -> void
{
  impl_mem_set<T, IsPOD<T>()>::Do(Num, Destination, Item);
}

} // namespace mtb
#endif // !defined(MTB_HEADER_memory)

#if defined(MTB_MEMORY_IMPLEMENTATION)

#if !defined(MTB_IMPL_memory)
#define MTB_IMPL_memory

#include <cstring>

auto mtb::
MemCopyBytes(memory_size Size, void* Destination, void const* Source)
  -> void
{
  // Using memmove so that Destination and Source may overlap.
  std::memmove(Destination, Source, ToBytes(Size));
}

auto mtb::
MemSetBytes(memory_size Size, void* Destination, int Value)
  -> void
{
  std::memset(Destination, Value, ToBytes(Size));
}

auto mtb::
MemEqualBytes(memory_size Size, void const* A, void const* B)
  -> bool
{
  return MemCompareBytes(Size, A, B) == 0;
}

auto mtb::
MemCompareBytes(memory_size Size, void const* A, void const* B)
  -> int
{
  return std::memcmp(A, B, ToBytes(Size));
}

auto mtb::
MemAreOverlappingBytes(memory_size SizeA, void const* A, memory_size SizeB, void const* B)
  -> bool
{
  auto LeftA = Reinterpret<size_t const>(A);
  auto RightA = LeftA + ToBytes(SizeA);

  auto LeftB = Reinterpret<size_t const>(B);
  auto RightB = LeftB + ToBytes(SizeB);

  return LeftB  >= LeftA && LeftB  <= RightA || // Check if LeftB  is in [A, A+SizeA]
         RightB >= LeftA && RightB <= RightA || // Check if RightB is in [A, A+SizeA]
         LeftA  >= LeftB && LeftA  <= RightB || // Check if LeftA  is in [B, B+SizeB]
         RightA >= LeftB && RightA <= RightB;   // Check if RightA is in [B, B+SizeB]
}

#endif // !defiend(MTB_IMPL_memory)
#endif // defined(MTB_MEMORY_IMPLEMENTATION)
