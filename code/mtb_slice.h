#if !defined(MTB_HEADER_mtb_slice)
#define MTB_HEADER_mtb_slice

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_SLICE_IMPLEMENTATION)
  #define MTB_SLICE_IMPLEMENTATION
#endif

#if defined(MTB_SLICE_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.h"
#include "mtb_assert.h"
#include "mtb_memory.h"

namespace mtb
{

constexpr size_t INVALID_INDEX = (size_t)-1;

template<typename T>
struct slice
{
  using ElementType = T;

  size_t Len;
  ElementType* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element. If `Len` is 0 or `Ptr` is `nullptr`, the slice is
  /// considered invalid (`false`).
  inline operator bool() const { return Len && Ptr; }

  /// Implicit conversion to const version.
  inline operator slice<T const>() const { return { Len, Ptr }; }

  /// Index operator to access elements of the slice.
  template<typename IndexType>
  inline auto
  operator[](IndexType Index) const
    -> decltype(Ptr[Index])
  {
    MTB_BoundsCheck(Index >= 0 && Index < Len);
    return Ptr[Index];
  }
};

template<>
struct slice<void const>
{
  using ElementType = void const;

  size_t Len;
  void const* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  inline operator bool() const { return Len && Ptr; }
};

template<>
struct slice<void>
{
  using ElementType = void;

  size_t Len;
  void* Ptr;

  /// Test whether this slice is valid or not.
  ///
  /// A slice is considered valid if it does not point to null and contains at
  /// least one element.
  inline operator bool() const { return Len && Ptr; }

  /// Implicit conversion to const version.
  inline operator slice<void const>() const { return { Len, Ptr }; }
};

template<typename T>
size_t
LengthOf(slice<T> const& A)
{
  return A.Len;
}

template<typename T>
size_t
SetLength(slice<T>& A, size_t NewLength)
{
  return Exchange(A.Len, NewLength);
}

template<typename T>
typename slice<T>::ElementType*
First(slice<T> A)
{
  return A.Ptr;
}

template<typename T>
typename slice<T>::ElementType*
Last(slice<T> A)
{
  return AddElementOffset(First(A), Max(size_t(1), LengthOf(A)) - 1);
}

template<typename T>
typename slice<T>::ElementType*
OnePastLast(slice<T> A)
{
  return AddElementOffset(First(A), LengthOf(A));
}

/// C++11 range API
template<typename T>
typename slice<T>::ElementType*
begin(slice<T> A)
{
  return First(A);
}

/// C++11 range API
template<typename T>
typename slice<T>::ElementType*
end(slice<T> A)
{
  return OnePastLast(A);
}

template<typename TargetType, typename SourceType>
slice<TargetType>
Reinterpret(slice<SourceType> A)
{
  return Slice(Reinterpret<TargetType*>(First(A)),
               Reinterpret<TargetType*>(OnePastLast(A)));
}

template<typename SourceType>
slice<SourceType const>
AsConst(slice<SourceType> A)
{
  return Slice(AsPtrToConst(First(A)),
               AsPtrToConst(OnePastLast(A)));
}

/// Concatenate two slices together.
///
/// \return The returned slice will be a subset of the given Buffer, which is
/// \used to write the actual result in.
template<typename ElementType>
slice<ElementType>
Concat(slice<ElementType const> Head, slice<ElementType const> Tail, slice<ElementType> Buffer)
{
  size_t BufferIndex{};
  size_t HeadIndex{};
  size_t TailIndex{};

  while(BufferIndex < LengthOf(Buffer) && HeadIndex < LengthOf(Head))
  {
    Buffer[BufferIndex++] = Head[HeadIndex++];
  }

  while(BufferIndex < LengthOf(Buffer) && TailIndex < LengthOf(Tail))
  {
    Buffer[BufferIndex++] = Tail[TailIndex++];
  }

  slice<ElementType> Result = Slice(Buffer, 0, BufferIndex);
  return Result;
}

/// Create a union of both input spans. The resulting slice will contain everything
template<typename ElementType>
constexpr slice<ElementType>
Union(slice<ElementType> SliceA, slice<ElementType> SliceB)
{
  // A union only makes sense when both slices are overlapping.
  return { Min(First(SliceA), First(SliceB)), Max(OnePastLast(SliceA), OnePastLast(SliceB)) };
}

template<typename ElementTypeA, typename ElementTypeB>
constexpr bool
AreDisjoint(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  return Last(SliceA) < First(SliceB) || First(SliceA) > Last(SliceB);
}

/// Whether SliceA and SliceB overlap.
/// \see Contains
template<typename ElementTypeA, typename ElementTypeB>
bool
AreOverlapping(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  auto UnionOfAB = Union(SliceA, SliceB);
  return Contains(UnionOfAB, SliceA) || Contains(UnionOfAB, SliceA);
}

/// Whether SliceA completely contains SliceB.
/// \see AreOverlapping
template<typename ElementTypeA, typename ElementTypeB>
constexpr bool
Contains(slice<ElementTypeA> SliceA, slice<ElementTypeB> SliceB)
{
  return First(SliceA) <= First(SliceB) && OnePastLast(SliceA) >= OnePastLast(SliceB);
}

template<typename ElementType>
constexpr slice<ElementType>
Slice(size_t Len, ElementType* Ptr)
{
  return { Len, Ptr };
}


/// FirstPtr and OnePastLastPtr must define valid slice boundaries for the
/// given type.
///
/// Here's an example of an invalid slice boundary:
///   Slice((int*)100, (int*)101);
///
/// The different between the two pointers above is 1 but these differences
/// must always be a multiple of sizeof(int).
template<typename T>
slice<T>
Slice(T* FirstPtr, T* OnePastLastPtr)
{
  auto OnePastLastPtr_ = NonVoidPtr(OnePastLastPtr);
  auto FirstPtr_       = NonVoidPtr(FirstPtr);

  #if MTB_IsOn(MTB_WantBoundsCheck)
  // Check whether FirstPtr and OnePastLastPtr define a valid slice boundary.
  {
    auto A = Reinterpret<size_t>(FirstPtr);
    auto B = Reinterpret<size_t>(OnePastLastPtr);
    // NOTE: Don't use Abs(B - A) here because we're dealing with unsigned values.
    auto Delta = Max(A, B) - Min(A, B);
    MTB_Require(Delta % SizeOf<T>() == 0);
  }
  #endif

  slice<T> Result;
  SetLength(Result, OnePastLastPtr_ <= FirstPtr_ ? 0 : OnePastLastPtr_ - FirstPtr_);
  Result.Ptr = FirstPtr;
  return Result;
}

template<typename T, size_t N>
constexpr slice<T>
Slice(T (&Array)[N])
{
  return { N, &Array[0] };
}

/// Create a char slice from a static char array, excluding '\0'.
template<size_t N>
constexpr slice<char const>
SliceFromString(char const(&StringLiteral)[N])
{
  return { N - 1, &StringLiteral[0] };
}

/// \param StringPtr Must be null-terminated.
slice<char const>
SliceFromString(char const* StringPtr);

/// \param StringPtr Must be null-terminated.
slice<char>
SliceFromString(char* StringPtr);

/// Custom string literal suffix.
/// Usage: slice<char const> Foo = "Foo"_S;
inline slice<char const>
operator "" _S(char const* StringPtr, size_t Num) { return Slice(Num, StringPtr); }

/// Creates a new slice from an existing slice.
///
/// \param InclusiveStartIndex The index to start slicing from.
/// \param ExclusiveEndIndex The index of the first excluded element.
template<typename ElementType, typename StartIndexType, typename EndIndexType>
slice<ElementType>
Slice(slice<ElementType> A, StartIndexType InclusiveStartIndex, EndIndexType ExclusiveEndIndex)
{
  MTB_ReleaseAssert(InclusiveStartIndex <= ExclusiveEndIndex);
  slice<ElementType> Result;
  SetLength(Result, ExclusiveEndIndex - InclusiveStartIndex);
  Result.Ptr = AddElementOffset(A.Ptr, InclusiveStartIndex);
  MTB_BoundsCheck(Contains(A, Result));
  return Result;
}

/// Creates a new slice from an existing one, trimming elements at the beginning.
template<typename ElementType, typename AmountType>
constexpr slice<ElementType>
TrimFront(slice<ElementType> A, AmountType Amount)
{
  return
  {
    Amount > LengthOf(A) ? 0 : LengthOf(A) - Amount,
    AddElementOffset(A.Ptr, Amount)
  };
}

/// Creates a new slice from an existing one, trimming elements at the beginning.
template<typename ElementType, typename AmountType>
constexpr slice<ElementType>
TrimBack(slice<ElementType> A, AmountType Amount)
{
  return
  {
    Amount > LengthOf(A) ? 0 : LengthOf(A) - Amount,
    A.Ptr
  };
}

template<typename T, typename... ArgTypes>
inline void
ConstructElements(slice<T> Destination, ArgTypes&&... Args)
{
  MemConstruct(LengthOf(Destination), Destination.Ptr, Forward<ArgTypes>(Args)...);
}

template<typename T>
inline void
DestructElements(slice<T> Destination)
{
  MemDestruct(LengthOf(Destination), Destination.Ptr);
}

template<typename T>
inline size_t
CopyElements(slice<T> Destination, slice<T const> Source)
{
  size_t const Amount = Min(LengthOf(Destination), LengthOf(Source));
  MemCopy(Amount, Destination.Ptr, Source.Ptr);
  return Amount;
}

template<typename T>
inline size_t
CopyConstructElements(slice<T> Destination, slice<T const> Source)
{
  size_t const Amount = Min(LengthOf(Destination), LengthOf(Source));
  MemCopyConstruct(Amount, Destination.Ptr, Source.Ptr);
  return Amount;
}

template<typename T>
inline size_t
MoveElements(slice<T> Destination, slice<T> Source)
{
  size_t const Amount = Min(LengthOf(Destination), LengthOf(Source));
  MemMove(Amount, Destination.Ptr, Source.Ptr);
  return Amount;
}

template<typename T>
inline size_t
MoveConstructElements(slice<T> Destination, slice<T> Source)
{
  size_t const Amount = Min(LengthOf(Destination), LengthOf(Source));
  MemMoveConstruct(Amount, Destination.Ptr, Source.Ptr);
  return Amount;
}

template<typename T, typename U>
inline void
SetElements(slice<T> Destination, U&& Item)
{
  MemSet(LengthOf(Destination), Destination.Ptr, Forward<U>(Item));
}

template<typename T, typename NeedleType>
size_t
CountUntil(slice<T const> Haystack, NeedleType const& Needle)
{
  size_t Index = 0;

  for(auto& Straw : Haystack)
  {
    if(Straw == Needle)
      return Index;
    ++Index;
  }

  return INVALID_INDEX;
}

/// Counts up until \c Predicate(ElementOfHaystack, Needle) returns \c true.
template<typename T, typename NeedleType, typename PredicateType>
size_t
CountUntil(slice<T const> Haystack, NeedleType const& Needle, PredicateType Predicate)
{
  size_t Index = 0;

  for(auto& Straw : Haystack)
  {
    if(Predicate(Straw, Needle))
      return Index;
    ++Index;
  }

  return INVALID_INDEX;
}

template<typename T, typename U>
bool
StartsWith(slice<T const> Slice, slice<U const> Sequence)
{
  size_t const Amount = Min(LengthOf(Slice), LengthOf(Sequence));

  for(size_t Index = 0; Index < Amount; ++Index)
  {
    if(Slice[Index] != Sequence[Index])
      return false;
  }

  return true;
}

template<typename T, typename NeedleType>
slice<T>
Find(slice<T> Haystack, NeedleType const& Needle)
{
  while(LengthOf(Haystack))
  {
    if(Haystack[0] == Needle)
      return Haystack;
    Haystack = TrimFront(Haystack, 1);
  }

  return Haystack;
}

template<typename T, typename NeedleType, typename PredicateType>
slice<T>
Find(slice<T> Haystack, NeedleType const& Needle, PredicateType Predicate)
{
  while(LengthOf(Haystack))
  {
    if(Predicate(Haystack[0], Needle))
      return Haystack;
    Haystack = TrimFront(Haystack, 1);
  }

  return Haystack;
}

template<typename T, typename NeedleType>
slice<T>
Find(slice<T> Haystack, slice<NeedleType> const& NeedleSequence)
{
  while(LengthOf(Haystack))
  {
    if(StartsWith(Haystack, NeedleSequence))
      return Haystack;
    Haystack = TrimFront(Haystack, 1);
  }

  return Haystack;
}

template<typename T>
void
ReverseElements(slice<T> SomeSlice)
{
  auto const NumSwaps = LengthOf(SomeSlice) / 2;
  for(size_t FrontIndex = 0; FrontIndex < NumSwaps; ++FrontIndex)
  {
    auto const BackIndex = LengthOf(SomeSlice) - FrontIndex - 1;
    Swap(SomeSlice[FrontIndex], SomeSlice[BackIndex]);
  }
}

/// Compares the contents of the two slices for equality.
///
/// Two slices are deemed equal if they have the same number of elements and
/// each individual element in A compares equal to the corresponding element
/// in B in the order they appear in.
template<typename ElementTypeA, typename ElementTypeB>
bool
operator ==(slice<ElementTypeA> A, slice<ElementTypeB> B)
{
  if(LengthOf(A) != LengthOf(B)) return false;

  auto A_ = NonVoidPtr(First(A));
  auto B_ = NonVoidPtr(First(B));
  // if(A_ == B_) return true;
  if(Coerce<size_t>(A_) == Coerce<size_t>(B_)) return true;

  auto NumElements = LengthOf(A);
  while(NumElements)
  {
    if(*A_ != *B_)
      return false;

    ++A_;
    ++B_;
    --NumElements;
  }

  return true;
}

template<typename ElementType>
bool
operator ==(slice<ElementType> Slice, nullptr_t)
{
  return !Cast<bool>(Slice);
}
template<typename ElementType>
bool
operator !=(slice<ElementType> Slice, nullptr_t)
{
  return Cast<bool>(Slice);
}

template<typename ElementType>
bool
operator ==(nullptr_t, slice<ElementType> Slice)
{
  return !(Slice == nullptr);
}
template<typename ElementType>
bool
operator !=(nullptr_t, slice<ElementType> Slice)
{
  return Slice != nullptr;
}

template<typename ElementTypeA, typename ElementTypeB>
bool
operator !=(slice<ElementTypeA> A, slice<ElementTypeB> B)
{
  return !(A == B);
}

template<typename ElementType>
constexpr size_t
ByteLengthOf(slice<ElementType> A)
{
  return LengthOf(A) * SizeOf<ElementType>();
}

} // namespace mtb
#endif // !defined(MTB_HEADER_mtb_slice)


// ==============
// Implementation
// ==============

#if defined(MTB_SLICE_IMPLEMENTATION)
#if !defined(MTB_IMPL_mtb_slice)
#define MTB_IMPL_mtb_slice

auto mtb::
SliceFromString(char const* StringPtr)
  -> slice<char const>
{
  auto Seek = StringPtr;
  size_t Count = 0;
  while(*Seek++) ++Count;
  return { Count, StringPtr };
}

auto mtb::
SliceFromString(char* StringPtr)
  -> slice<char>
{
  auto Constified = Coerce<char const*>(StringPtr);
  auto ConstResult = SliceFromString(Constified);
  slice<char> Result;
  SetLength(Result, LengthOf(ConstResult));
  Result.Ptr = Coerce<char*>(ConstResult.Ptr);
  return Result;
}

#endif // !defined(MTB_IMPL_mtb_slice)
#endif // defined(MTB_SLICE_IMPLEMENTATION)
