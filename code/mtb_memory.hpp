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

/// \defgroup Memory manipulation functions
///
/// Provides functions to work on chunks of memory.
///
/// Unlike C standard functions such as memcpy and memset, these functions
/// respect the type of the input objects. Refer to the table below to find
/// which C standard functionality is covered by which of the functions
/// defined here.
///
/// C Standard Function | Untyped/Bytes                        | Typed
/// ------------------- | ------------------------------------ | -----
/// memcopy, memmove    | mtb_CopyBytes                        | mtb_CopyElements, mtb_CopyConstructElements, mtb_MoveElements, mtb_MoveConstructElements
/// memset              | mtb_SetBytes                         | mtb_SetElements, mtb_ConstructElements
/// memcmp              | mtb_CompareBytes, mtb_AreBytesEqual  | -
///
///
/// All functions are optimized for POD types.
///
/// @{

/// Copy Size from Source to Destination.
///
/// Destination and Source may overlap.
void
mtb_CopyBytes(size_t Size, void* Destination, void const* Source);

/// Fill NumBytes in Destination with the value
void
mtb_SetBytes(size_t Size, void* Destination, int Value);

bool
mtb_AreBytesEqual(size_t Size, void const* A, void const* B);

int
mtb_CompareBytes(size_t Size, void const* A, void const* B);


/// Calls the constructor of all elements in Destination with Args.
///
/// Args may be empty in which case all elements get default-initialized.
template<typename T, typename... ArgTypes>
void
mtb_ConstructElements(size_t Num, T* Destination, ArgTypes&&... Args);

/// Destructs all elements in Destination.
template<typename T>
void
mtb_DestructElements(size_t Num, T* Destination);

/// Copy all elements from Source to Destination.
///
/// Destination and Source may overlap.
template<typename T>
void
mtb_CopyElements(size_t Num, T* Destination, T const* Source);

/// Copy all elements from Source to Destination using T's constructor.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
mtb_CopyConstructElements(size_t Num, T* Destination, T const* Source);

/// Move all elements from Source to Destination using T's constructor.
///
/// Destination and Source may overlap.
template<typename T>
void
mtb_MoveElements(size_t Num, T* Destination, T* Source);

/// Move all elements from Source to Destination using T's constructor and destruct Source afterwards.
///
/// Destination and Source may NOT overlap. Destination is assumed to be
/// uninitialized.
template<typename T>
void
mtb_MoveConstructElements(size_t Num, T* Destination, T* Source);

/// Assign Item to all elements in Destination.
template<typename T>
void
mtb_SetElements(size_t Num, T* Destination, T const& Item);

bool
mtb_ImplTestMemoryOverlap(size_t SizeA, void const* A, size_t SizeB, void const* B);

template<typename TA, typename TB>
bool
mtb_TestMemoryOverlap(size_t NumA, TA const* A, size_t NumB, TB const* B)
{
  return mtb_ImplTestMemoryOverlap(NumA * mtb_SafeSizeOf<TA>(), reinterpret_cast<void const*>(A),
                                   NumB * mtb_SafeSizeOf<TB>(), reinterpret_cast<void const*>(B));
}


//
// Implementation Details
//

// mtb_ConstructElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_construct_elements
{
  template<typename... ArgTypes>
  MTB_INLINE static void
  Do(size_t Num, T* Destination, ArgTypes&&... Args)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(mtb_Forward<ArgTypes>(Args)...);
    }
  }
};

template<typename T>
struct mtb_impl_construct_elements<T, true>
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination)
  {
    mtb_SetBytes(Num * mtb_SafeSizeOf<T>(), Destination, 0);
  }

  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    // Blit Item over each element of Destination.
    for(size_t Index = 0; Index < Num; ++Index)
    {
      mtb_CopyBytes(mtb_SafeSizeOf<T>(), &Destination[Index], &Item);
    }
  }
};

template<typename T, typename... ArgTypes>
MTB_INLINE auto
mtb_ConstructElements(size_t Num, T* Destination, ArgTypes&&... Args)
  -> void
{
  mtb_impl_construct_elements<T, MTB_IsPod(T)>::Do(Num, Destination, mtb_Forward<ArgTypes>(Args)...);
}


// mtb_DestructElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_destruct_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index].~T();
    }
  }
};

template<typename T>
struct mtb_impl_destruct_elements<T, true>
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination)
  {
    // Nothing to do for POD types.
  }
};

template<typename T>
MTB_INLINE auto
mtb_DestructElements(size_t Num, T* Destination)
  -> void
{
  mtb_impl_destruct_elements<T, MTB_IsPod(T)>::Do(Num, Destination);
}


// mtb_CopyElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_copy_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    if(Destination == Source)
      return;

    if(mtb_TestMemoryOverlap(Num, Destination, Num, Source) && Destination < Source)
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
struct mtb_impl_copy_elements<T, true>
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    mtb_CopyBytes(mtb_SafeSizeOf<T>() * Num, Destination, Source);
  }
};

template<typename T>
MTB_INLINE auto
mtb_CopyElements(size_t Num, T* Destination, T const* Source)
  -> void
{
  mtb_impl_copy_elements<T, MTB_IsPod(T)>::Do(Num, Destination, Source);
}


// mtb_CopyConstructElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_copy_construct_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_AssertDebug(!mtb_TestMemoryOverlap(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(Source[Index]);
    }
  }
};

template<typename T>
struct mtb_impl_copy_construct_elements<T, true>
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_AssertDebug(!mtb_TestMemoryOverlap(Num, Destination, Num, Source));

    mtb_CopyElements(Num, Destination, Source);
  }
};

template<typename T>
MTB_INLINE auto
mtb_CopyConstructElements(size_t Num, T* Destination, T const* Source)
  -> void
{
  mtb_impl_copy_construct_elements<T, MTB_IsPod(T)>::Do(Num, Destination, Source);
}


// mtb_MoveElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_move_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T* Source)
  {
    if(Destination == Source)
      return;

    if(mtb_TestMemoryOverlap(Num, Destination, Num, Source))
    {
      if(Destination < Source)
      {
        // Move forward
        for(size_t Index = 0; Index < Num; ++Index)
        {
          Destination[Index] = mtb_Move(Source[Index]);
        }

        // Destroy the remaining elements in the back.
        size_t const NumToDestruct = Source - Destination;
        mtb_DestructElements(NumToDestruct, Source + (Num - NumToDestruct));
      }
      else
      {
        // Move backward
        for(size_t Index = Num; Index > 0;)
        {
          --Index;
          Destination[Index] = mtb_Move(Source[Index]);
        }

        // Destroy the remaining elements in the front.
        size_t const NumToDestruct = Destination - Source;
        mtb_DestructElements(NumToDestruct, Source);
      }
    }
    else
    {
      // Straight forward: Move one by one, then destruct all in Source.
      for(size_t Index = 0; Index < Num; ++Index)
      {
        Destination[Index] = mtb_Move(Source[Index]);
      }
      mtb_DestructElements(Num, Source);
    }
  }
};

template<typename T>
struct mtb_impl_move_elements<T, true> : public mtb_impl_copy_elements<T, true> {};

template<typename T>
MTB_INLINE auto
mtb_MoveElements(size_t Num, T* Destination, T* Source)
  -> void
{
  mtb_impl_move_elements<T, MTB_IsPod(T)>::Do(Num, Destination, Source);
}


// mtb_MoveConstructElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_move_construct_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T* Source)
  {
    // When using the constructor, overlapping is not allowed.
    MTB_AssertDebug(!mtb_TestMemoryOverlap(Num, Destination, Num, Source));

    for(size_t Index = 0; Index < Num; ++Index)
    {
      new (&Destination[Index]) T(mtb_Move(Source[Index]));
    }
    mtb_DestructElements(Num, Source);
  }
};

template<typename T>
struct mtb_impl_move_construct_elements<T, true>
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const* Source)
  {
    // When using the constructor, overlapping is not allowed. Even though in
    // the POD case here it doesn't make a difference, it might help to catch
    // bugs since this can't be intentional.
    MTB_AssertDebug(!mtb_TestMemoryOverlap(Num, Destination, Num, Source));

    mtb_CopyElements(Num, Destination, Source);
  }
};

template<typename T>
MTB_INLINE auto
mtb_MoveConstructElements(size_t Num, T* Destination, T* Source)
  -> void
{
  mtb_impl_move_construct_elements<T, MTB_IsPod(T)>::Do(Num, Destination, Source);
}


// mtb_SetElements

template<typename T, bool TIsPlainOldData = false>
struct mtb_impl_set_elements
{
  MTB_INLINE static void
  Do(size_t Num, T* Destination)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = {};
    }
  }

  MTB_INLINE static void
  Do(size_t Num, T* Destination, T const& Item)
  {
    for(size_t Index = 0; Index < Num; ++Index)
    {
      Destination[Index] = Item;
    }
  }
};

template<typename T>
struct mtb_impl_set_elements<T, true> : public mtb_impl_construct_elements<T, true> {};

template<typename T>
MTB_INLINE auto
mtb_SetElements(size_t Num, T* Destination)
  -> void
{
  mtb_impl_set_elements<T, MTB_IsPod(T)>::Do(Num, Destination);
}

template<typename T>
MTB_INLINE auto
mtb_SetElements(size_t Num, T* Destination, T const& Item)
  -> void
{
  mtb_impl_set_elements<T, MTB_IsPod(T)>::Do(Num, Destination, Item);
}

#endif // !defined(MTB_HEADER_mtb_memory)

#if defined(MTB_MEMORY_IMPLEMENTATION)

#if !defined(MTB_IMPL_mtb_memory)
#define MTB_IMPL_mtb_memory

#include <string.h>

void
mtb_CopyBytes(size_t Size, void* Destination, void const* Source)
{
  // Using memmove so that Destination and Source may overlap.
  memmove(Destination, Source, Size);
}

void
mtb_SetBytes(size_t Size, void* Destination, int Value)
{
  memset(Destination, Value, Size);
}

bool
mtb_AreBytesEqual(size_t Size, void const* A, void const* B)
{
  return mtb_CompareBytes(Size, A, B) == 0;
}

int
mtb_CompareBytes(size_t Size, void const* A, void const* B)
{
  return memcmp(A, B, Size);
}

bool
mtb_ImplTestMemoryOverlap(size_t SizeA, void const* A, size_t SizeB, void const* B)
{
  size_t LeftA = (size_t)A;
  size_t RightA = LeftA + SizeA;

  size_t LeftB = (size_t)B;
  size_t RightB = LeftB + SizeB;

  return LeftB  >= LeftA && LeftB  <= RightA || // Check if LeftB  is in [A, A+SizeA]
         RightB >= LeftA && RightB <= RightA || // Check if RightB is in [A, A+SizeA]
         LeftA  >= LeftB && LeftA  <= RightB || // Check if LeftA  is in [B, B+SizeB]
         RightA >= LeftB && RightA <= RightB;   // Check if RightA is in [B, B+SizeB]
}

#endif // !defiend(MTB_IMPL_mtb_memory)
#endif // defined(MTB_MEMORY_IMPLEMENTATION)
