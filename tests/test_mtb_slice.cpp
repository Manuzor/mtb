#include "mtb_slice.hpp"

#include "catch.hpp"

TEST_CASE("Slice creation", "[Slice]")
{
  using namespace mtb;

  SECTION("Uninitialized slices ")
  {
    slice<int> Foo{};
    REQUIRE(LengthOf(Foo) == 0);
    REQUIRE(Foo.Ptr == nullptr);
  }

  SECTION("From number and pointer")
  {
    int Foo = 42;
    auto FooSlice = Slice(1, &Foo);
    REQUIRE(LengthOf(FooSlice) == 1);
    REQUIRE(FooSlice[0] == 42);
  }

  SECTION("From begin and end pointer")
  {
    REQUIRE( LengthOf(Slice<int>(nullptr, nullptr)) == 0 );
    REQUIRE( Slice<int>(nullptr, nullptr).Ptr == nullptr );

    int Foos[] = {0, 1, 2};

    auto Foo1 = Slice(&Foos[0], &Foos[0]);
    REQUIRE(LengthOf(Foo1) == 0);
    REQUIRE(Foo1.Ptr == &Foos[0]);

    auto Foo2 = Slice(&Foos[0], &Foos[3]);
    REQUIRE(LengthOf(Foo2) == 3);
    REQUIRE(Foo2[0] == Foos[0]);
    REQUIRE(Foo2[1] == Foos[1]);
    REQUIRE(Foo2[2] == Foos[2]);
  }

  SECTION("From static array")
  {
    int Foo[] = {0, 1, 2};
    auto Bar = Slice(Foo);
    REQUIRE(LengthOf(Bar) == LengthOf(Foo));
    REQUIRE(Bar.Ptr == &Foo[0]);
  }

  SECTION("From const string")
  {
    const char CharArray[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
    const char* String = &CharArray[0];

    auto Foo = SliceFromString(CharArray);
    REQUIRE(LengthOf(Foo) == LengthOf(CharArray) - 1);
    REQUIRE(Foo.Ptr == &CharArray[0]);

    auto Bar = SliceFromString(String);
    REQUIRE(LengthOf(Bar) == LengthOf(CharArray) - 1);
    REQUIRE(Bar.Ptr == String);
  }

  SECTION("From mutable string")
  {
    char CharArray[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
    char* String = &CharArray[0];

    auto Foo = SliceFromString(CharArray);
    REQUIRE(LengthOf(Foo) == LengthOf(CharArray) - 1);
    REQUIRE(Foo.Ptr == &CharArray[0]);

    auto Bar = SliceFromString(String);
    REQUIRE(LengthOf(Bar) == LengthOf(CharArray) - 1);
    REQUIRE(Bar.Ptr == String);
  }

  SECTION("From two indices")
  {
    int DataArray[]{ 0, 1, 2 };
    auto Data = Slice(DataArray);

    auto Foo = Slice(Data, 0, 0);
    REQUIRE( LengthOf(Foo) == 0 );
    REQUIRE( Foo.Ptr == Data.Ptr );

    auto Bar = Slice(Data, 0, 1);
    REQUIRE( LengthOf(Bar) == 1 );
    REQUIRE( Bar.Ptr == Data.Ptr );

    auto Baz = Slice(Bar, 0, 1);
    REQUIRE( LengthOf(Baz) == 1 );
    REQUIRE( Baz.Ptr == Data.Ptr );

    auto Baar = Slice(Data, 1, 3);
    REQUIRE( LengthOf(Baar) == 2 );
    REQUIRE( Baar.Ptr == Data.Ptr + 1 );
  }
}

TEST_CASE("Slice implicit bool conversion", "[Slice]")
{
  using namespace mtb;

  slice<int> Foo{};
  REQUIRE(Cast<bool>(Foo) == false);

  SECTION("Set Num")
  {
    Foo.Len = 42;
    REQUIRE(Cast<bool>(Foo) == false);
  }

  SECTION("Set Ptr")
  {
    Foo.Ptr = Coerce<int*, u64>(42);
    REQUIRE(Cast<bool>(Foo) == false);
  }
}

TEST_CASE("Slices of void", "[Slice]")
{
  using namespace mtb;

  // slice<void> Foo;
  // slice<void const> Bar;
}

TEST_CASE("Slice casting", "[Slice]")
{
  using namespace mtb;

  SECTION("Reinterpretation")
  {
    float Foo = 3.1415f;
    auto Bar = Slice(1, &Foo);
    auto Baz = Reinterpret<int>(Bar);
    REQUIRE(Baz[0] == *reinterpret_cast<int*>(&Bar[0]));
  }
}

TEST_CASE("Slice Equality", "[Slice]")
{
  using namespace mtb;

  SECTION("Both Empty")
  {
    REQUIRE(slice<void>()  == slice<void>());
    REQUIRE(slice<i32>() == slice<i32>());
    REQUIRE(slice<u16>() == slice<i32>());
  }

  SECTION("Single Value")
  {
    int A = 42;
    int B = 123;
    int C = 42;
    auto Foo = Slice(1, &A);
    auto Bar = Slice(1, &B);
    auto Baz = Slice(1, &C);

    REQUIRE(Foo == Foo);
    REQUIRE(Foo != Bar);
    REQUIRE(Foo == Baz);
  }

  SECTION("From Same Data")
  {
    int Data[] = { 1, 2, 1, 2, };
    auto Foo = Slice(2, &Data[0]);
    auto Bar = Slice(2, &Data[1]);
    auto Baz = Slice(2, &Data[2]);

    REQUIRE(Foo == Foo);
    REQUIRE(Foo != Bar);
    REQUIRE(Foo == Baz);
  }

  SECTION("Same length, different content but same first element")
  {
    auto Foo = SliceFromString("ABC");
    auto Bar = SliceFromString("ABD");

    REQUIRE(Foo != Bar);
  }
}

TEST_CASE("Slice Searching", "[Slice]")
{
  using namespace mtb;

  int Ints[] = { 0, 1, 2, 3, 4, 5, 6 };
  auto Foo = Slice(Ints);

  SECTION("CountUntil")
  {
    REQUIRE( CountUntil(Slice(0, (int const*)nullptr), 42) == INVALID_INDEX );

    REQUIRE( CountUntil(AsConst(Foo), 0) == 0 );
    REQUIRE( CountUntil(AsConst(Foo), 2) == 2 );
    REQUIRE( CountUntil(AsConst(Foo), 6) == 6 );
    REQUIRE( CountUntil(AsConst(Foo), 7) == INVALID_INDEX );
  }

  SECTION("CountUntil with predicate")
  {
    REQUIRE( CountUntil(Slice<int const>((size_t)0, nullptr), 42) == INVALID_INDEX );

    REQUIRE( CountUntil(AsConst(Foo), -1, [](int A, int B){ return A == 0; }) == 0 );
    REQUIRE( CountUntil(AsConst(Foo), -1, [](int A, int B){ return A == 2; }) == 2 );
    REQUIRE( CountUntil(AsConst(Foo), -1, [](int A, int B){ return A == 6; }) == 6 );
    REQUIRE( CountUntil(AsConst(Foo), -1, [](int A, int B){ return A == 7; }) == INVALID_INDEX );
  }
}

TEST_CASE("Slice Misc", "[Slice]")
{
  using namespace mtb;

  slice<int> Foo{};
  Foo.Len = 42;
  REQUIRE( ByteLengthOf(Foo) == 42 * sizeof(int) );

  slice<void> Bar{};
  SetLength(Bar, 1337);
  REQUIRE( ByteLengthOf(Bar) == 1337 );
}
