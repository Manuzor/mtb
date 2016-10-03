#if !defined(MTB_HEADER_mtb_memory)
#define MTB_HEADER_mtb_memory

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_MEMORY_IMPLEMENTATION)
  #define MTB_MEMORY_IMPLEMENTATION
#endif

#if defined(MTB_MEMORY_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.hpp"
#include "mtb_assert.hpp"

#include <new>

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
/// C Standard Function | Untyped/Bytes                | Typed
/// ------------------- | ---------------------------- | -----
/// memcopy, memmove    | CopyBytes                    | CopyElements, CopyConstructElements, MoveElements, MoveConstructElements
/// memset              | SetBytes                     | SetElements, ConstructElements
/// memcmp              | CompareBytes, AreBytesEqual  | -
///
///
/// All functions are optimized for POD types.
///
/// @{

/// Copy Size from Source to Destination.
///
/// Destination and Source may overlap.
void
CopyBytes(size_t Size, void* Destination, void const* Source);

/// Fill NumBytes in Destination with the value
void
SetBytes(size_t Size, void* Destination, int Value);

bool
AreBytesEqual(size_t Size, void const* A, void const* B);

int
CompareBytes(size_t Size, void const* A, void const* B);


/// Calls the constructor of all elements in Destination with Args.
///
/// Args may be empty in which case all elements get default-initialized.
template<typename T, typename... ArgTypes>
void
ConstructElements(size_t Num, T* Destination, ArgTypes&&... Args);

/// Destructs all elements in Destination.
template<typename T>
void
DestructElements(size_t Num, T* Destination);

/// Copy all elements from Source to Destination.
///
/// Destination and Source may overlap.
template<typename T>
void
CopyElements(size_t Num, T* Destination, T const* Source);

/// Copy all elements from Source to Destination using T's constructor.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
CopyConstructElements(size_t Num, T* Destination, T const* Source);

/// Move all elements from Source to Destination using T's constructor.
///
/// Destination and Source may overlap.
template<typename T>
void
MoveElements(size_t Num, T* Destination, T* Source);

/// Move all elements from Source to Destination using T's constructor and destruct Source afterwards.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
MoveConstructElements(size_t Num, T* Destination, T* Source);

/// Assign the default value of T to all elements in Destination.
template<typename T>
void
SetElementsToDefault(size_t Num, T* Destination);

/// Assign Item to all elements in Destination.
template<typename T>
void
SetElements(size_t Num, T* Destination, T const& Item);

bool
ImplTestMemoryOverlap(size_t SizeA, void const* A, size_t SizeB, void const* B);

template<typename TA, typename TB>
bool
TestMemoryOverlap(size_t NumA, TA const* A, size_t NumB, TB const* B)
{
  return ImplTestMemoryOverlap(NumA * SizeOf<TA>(), Reinterpret<void const*>(A),
                               NumB * SizeOf<TB>(), Reinterpret<void const*>(B));
}


//
// Implementation Details
//

// ConstructElements

template<typename T, bool TIsPlainOldData = false>
struct impl_construct_elements
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
struct impl_construct_elements<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    SetBytes(Num * SizeOf<T>(), Destination, 0);
  }

  MTB_Inline static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    // Blit Item over each element of Destination.
    for(size_t Index = 0; Index < Num; ++Index)
    {
      CopyBytes(SizeOf<T>(), &Destination[Index], &Item);
    }
  }
};

template<typename T, typename... ArgTypes>
MTB_Inline auto
ConstructElements(size_t Num, T* Destination, ArgTypes&&... Args)
  -> void
{
  impl_construct_elements<T, IsPOD<T>()>::Do(Num, Destination, Forward<ArgTypes>(Args)...);
}


// DestructElements

template<typename T, bool TIsPlainOldData = false>
struct impl_destruct_elements
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
struct impl_destruct_elements<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination)
  {
    // Nothing to do for POD types.
  }
};

template<typename T>
MTB_Inline auto
DestructElements(size_t Num, T* Destination)
  -> void
{
  impl_destruct_elements<T, IsPOD<T>()>::Do(Num, Destination);
}


// CopyElements

template<typename T, bool TIsPlainOldData = false>
struct impl_copy_elements
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    if(Destination == Source)
      return;

    if(TestMemoryOverlap(Num, Destination, Num, Source) && Destination < Source)
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
struct impl_copy_elements<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    CopyBytes(SizeOf<T>() * Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
CopyElements(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_copy_elements<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// CopyConstructElements

template<typename T, bool TIsPlainOldData = false>
struct impl_copy_construct_elements
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_DebugAssert(!TestMemoryOverlap(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Source[Index]);
    }
  }
};

template<typename T>
struct impl_copy_construct_elements<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_DebugAssert(!TestMemoryOverlap(Num, Destination, Num, Source));

    CopyElements(Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
CopyConstructElements(size_t Num, T* Destination, T const* Source)
  -> void
{
  impl_copy_construct_elements<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MoveElements

template<typename T, bool TIsPlainOldData = false>
struct impl_move_elements
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    if(Destination == Source)
      return;

    if(TestMemoryOverlap(Num, Destination, Num, Source))
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
        DestructElements(NumToDestruct, AddElementOffset(Source, Num - NumToDestruct));
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
        DestructElements(NumToDestruct, Source);
      }
    }
    else
    {
      // Straight forward: Move one by one, then destruct all in Source.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = Move(Source[Index]);
      }
      DestructElements(Num, Source);
    }
  }
};

template<typename T>
struct impl_move_elements<T, true> : public impl_copy_elements<T, true> {};

template<typename T>
MTB_Inline auto
MoveElements(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_move_elements<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// MoveConstructElements

template<typename T, bool TIsPlainOldData = false>
struct impl_move_construct_elements
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_DebugAssert(!TestMemoryOverlap(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Move(Source[Index]));
    }
    DestructElements(Num, Source);
  }
};

template<typename T>
struct impl_move_construct_elements<T, true>
{
  MTB_Inline static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_DebugAssert(!TestMemoryOverlap(Num, Destination, Num, Source));

    CopyElements(Num, Destination, Source);
  }
};

template<typename T>
MTB_Inline auto
MoveConstructElements(size_t Num, T* Destination, T* Source)
  -> void
{
  impl_move_construct_elements<T, IsPOD<T>()>::Do(Num, Destination, Source);
}


// SetElements

template<typename T, bool TIsPlainOldData = false>
struct impl_set_elements
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
struct impl_set_elements<T, true> : public impl_construct_elements<T, true> {};

template<typename T>
MTB_Inline auto
SetElements(size_t Num, T* Destination)
  -> void
{
  impl_set_elements<T, IsPOD<T>()>::Do(Num, Destination);
}

template<typename T>
MTB_Inline auto
SetElements(size_t Num, T* Destination, T const& Item)
  -> void
{
  impl_set_elements<T, IsPOD<T>()>::Do(Num, Destination, Item);
}

} // namespace mtb
#endif // !defined(MTB_HEADER_mtb_memory)

#if defined(MTB_MEMORY_IMPLEMENTATION)

#if !defined(MTB_IMPL_mtb_memory)
#define MTB_IMPL_mtb_memory

#include <cstring>

auto mtb::
CopyBytes(size_t Size, void* Destination, void const* Source)
  -> void
{
  // Using memmove so that Destination and Source may overlap.
  std::memmove(Destination, Source, Size);
}

auto mtb::
SetBytes(size_t Size, void* Destination, int Value)
  -> void
{
  std::memset(Destination, Value, Size);
}

auto mtb::
AreBytesEqual(size_t Size, void const* A, void const* B)
  -> bool
{
  return CompareBytes(Size, A, B) == 0;
}

auto mtb::
CompareBytes(size_t Size, void const* A, void const* B)
  -> int
{
  return std::memcmp(A, B, Size);
}

auto mtb::
ImplTestMemoryOverlap(size_t SizeA, void const* A, size_t SizeB, void const* B)
  -> bool
{
  auto LeftA = Reinterpret<size_t const>(A);
  auto RightA = LeftA + SizeA;

  auto LeftB = Reinterpret<size_t const>(B);
  auto RightB = LeftB + SizeB;

  return LeftB  >= LeftA && LeftB  <= RightA || // Check if LeftB  is in [A, A+SizeA]
         RightB >= LeftA && RightB <= RightA || // Check if RightB is in [A, A+SizeA]
         LeftA  >= LeftB && LeftA  <= RightB || // Check if LeftA  is in [B, B+SizeB]
         RightA >= LeftB && RightA <= RightB;   // Check if RightA is in [B, B+SizeB]
}

#endif // !defiend(MTB_IMPL_mtb_memory)
#endif // defined(MTB_MEMORY_IMPLEMENTATION)
