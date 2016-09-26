#include "mtb.hpp"

#include "catch.hpp"

static_assert(mtb::IsPOD<void>(), "Type 'void' must be considered POD.");
static_assert(mtb::IsPOD<void const>(), "Type 'void const' must be considered POD.");
static_assert(mtb::IsNaN(mtb::NaN<float>()), "NaN() or IsNaN() aren't working properly.");
static_assert(mtb::IsNaN(mtb::NaN<double>()), "NaN() or IsNaN() aren't working properly.");

TEST_CASE("MTB_Defer", "[Common]")
{
  using namespace mtb;

  SECTION("POD")
  {
    int Value = 0;

    MTB_Defer [&](){ REQUIRE(Value == 3); };
    MTB_Defer [&](){ ++Value; };
    MTB_Defer [&](){ ++Value; };
    MTB_Defer [=](){ REQUIRE( Value == 0 ); };
    MTB_Defer [&](){ ++Value; };
  }

  SECTION("Complex capture")
  {
    struct foo
    {
      int Value = 0;
      foo() = default;
      foo(foo const& Other) { FAIL( "Copy constructor is not supposed to be called." ); }
      ~foo() { REQUIRE( Value == 42 ); }
    };

    struct bar
    {
      int Value = 1337;
      bar() = default;
      ~bar() { REQUIRE( Value == 1337 ); }
    };

    foo Foo;
    bar Bar;

    MTB_Defer [&](){ Foo.Value = 42; Bar = {}; };
    MTB_Defer [Bar](){ REQUIRE( Bar.Value == 1337 ); };
    MTB_Defer [&Bar](){ REQUIRE( Bar.Value == 123 ); };
    MTB_Defer [=, &Foo](){ REQUIRE( Foo.Value == 3 ); REQUIRE( Bar.Value == 1337 ); };

    Foo.Value = 3;

    Bar.Value = 123;
  }
}

TEST_CASE("Byte Sizes", "[Common]")
{
  using namespace mtb;

  REQUIRE( ToBytes(KiB(3)) == 3 * 1024ULL );
  REQUIRE( ToBytes(MiB(3)) == 3 * 1024ULL * 1024ULL );
  REQUIRE( ToBytes(GiB(3)) == 3 * 1024ULL * 1024ULL * 1024ULL );
  REQUIRE( ToBytes(TiB(3)) == 3 * 1024ULL * 1024ULL * 1024ULL * 1024ULL );

  REQUIRE( ToBytes(KB(3)) == 3 * 1000ULL );
  REQUIRE( ToBytes(MB(3)) == 3 * 1000ULL * 1000ULL );
  REQUIRE( ToBytes(GB(3)) == 3 * 1000ULL * 1000ULL * 1000ULL );
  REQUIRE( ToBytes(TB(3)) == 3 * 1000ULL * 1000ULL * 1000ULL * 1000ULL );

  REQUIRE( ToKiB(Bytes(512)) == 0.5f );
  REQUIRE( ToKiB<u64>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<u32>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<u16>(Bytes(512)) == 0 );
  REQUIRE( ToKiB<u8>(Bytes(512)) == 0 );

  REQUIRE( ToKiB(Bytes(2 * 1024)) == 2.0f );
  REQUIRE( ToKiB<u64>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<u32>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<u16>(Bytes(2 * 1024)) == 2 );
  REQUIRE( ToKiB<u8>(Bytes(2 * 1024)) == 2 );
}

TEST_CASE("Length of static arrays", "[Common]")
{
  using namespace mtb;

  SECTION("POD array")
  {
    int Array[42];
    REQUIRE(Length(Array) == 42);
  }

  SECTION("String literal")
  {
    REQUIRE(Length("Foo") == 3 + 1);
  }
}

TEST_CASE("Mem offsets", "[Common]")
{
  using namespace mtb;

  i32 Data = 42;
  auto DataOffset = (i64)&Data;
  auto Foo = (i32*)&Data;
  auto Bar = (void*)&Data;

  SECTION("Byte offset")
  {
    REQUIRE(MemAddByteOffset(Foo,  2) == (i32*)( (char*)&Data + 2 ));
    REQUIRE(MemAddByteOffset(Foo, -2) == (i32*)( (char*)&Data - 2 ));

    REQUIRE(MemAddByteOffset(Bar,  2) == (char*)&Data + 2);
    REQUIRE(MemAddByteOffset(Bar, -2) == (char*)&Data - 2);

    REQUIRE(MemAddByteOffset(Foo, -DataOffset)     == nullptr);
    REQUIRE(MemAddByteOffset(Foo, -DataOffset - 1) == (i32*)( (char*)nullptr - 1 ));
  }

  SECTION("Sized offset")
  {
    REQUIRE(MemAddOffset(Foo,  2) == Foo + 2);
    REQUIRE(MemAddOffset(Foo, -2) == Foo - 2);

    REQUIRE(MemAddOffset(Bar,  2) == (char*)Bar + 2);
    REQUIRE(MemAddOffset(Bar, -2) == (char*)Bar - 2);
  }
}

struct CastDataInt
{
  int Data = 42;
};

struct CastDataFloat
{
  float Data = 0.5f;
  operator CastDataInt() { return {}; }
};

namespace casts
{
  int helper(int&)       { return 42; }
  int helper(int const&) { return 64; }
  int helper(int*)       { return 128; }
  int helper(int const*) { return 512; }
}

TEST_CASE("Cast", "[Common]")
{
  using namespace mtb;

  REQUIRE(Cast<float>(1) == 1.0f);
  REQUIRE(Cast<CastDataInt>(CastDataFloat()).Data == 42);
}

TEST_CASE("Reinterpret", "[Common]")
{
  using namespace mtb;

  int Data;
  REQUIRE(Reinterpret<void*>(&Data) == (void*)&Data);
}

TEST_CASE("Coerce", "[Common]")
{
  using namespace mtb;

  REQUIRE(Coerce<float>((int)42) == (float)((int)42));
}

TEST_CASE("AsConst", "[Common]")
{
  using namespace mtb;

  int Data;
  REQUIRE(casts::helper(Data) == 42);
  REQUIRE(casts::helper(AsConst(Data)) == 64);
  REQUIRE(casts::helper(&Data) == 128);
  REQUIRE(casts::helper(AsPtrToConst(&Data)) == 512);
}

TEST_CASE("IsPowerOfTwo", "[Common]")
{
  using namespace mtb;

  size_t Iteration = 0;
  for(size_t Index = 0; Index < NumBits<size_t>(); ++Index)
  {
    CAPTURE( Index );
    auto Integer = size_t(1) << Index;
    CAPTURE( Integer );
    REQUIRE( IsPowerOfTwo(Integer) );
  }

  auto NegativeTest = [](size_t Argument)
  {
    CAPTURE(Argument);
    REQUIRE( !IsPowerOfTwo(Argument) );
  };

  NegativeTest( 3 );
  NegativeTest( 5 );
  NegativeTest( 6 );
  NegativeTest( 12 );
  NegativeTest( 1024-1 );
}

TEST_CASE("Min", "[Common]")
{
  using namespace mtb;

  REQUIRE(Min( 0,  1) == 0);
  REQUIRE(Min( 1,  0) == 0);
  REQUIRE(Min(-1,  0) == -1);
  REQUIRE(Min( 0, -1) == -1);

  REQUIRE(Min(u64(0), float(1)) == u64(0));
  REQUIRE(Min(u64(1), float(0)) == u64(0));
}

TEST_CASE("Max", "[Common]")
{
  using namespace mtb;

  REQUIRE(Max( 0,  1) == 1);
  REQUIRE(Max( 1,  0) == 1);
  REQUIRE(Max(-1,  0) == 0);
  REQUIRE(Max( 0, -1) == 0);

  REQUIRE(Max(u64(0), float(1)) == u64(1));
  REQUIRE(Max(u64(1), float(0)) == u64(1));
}

TEST_CASE("Clamp", "[Common]")
{
  using namespace mtb;

  REQUIRE(Clamp(10, 5, 15) == 10);
  REQUIRE(Clamp( 5, 5, 15) ==  5);
  REQUIRE(Clamp(15, 5, 15) == 15);
  REQUIRE(Clamp(20, 5, 15) == 15);
  REQUIRE(Clamp( 0, 5, 15) ==  5);

  REQUIRE(Clamp(10, 15, 5) ==  10);
}

TEST_CASE("Wrap", "[Common]")
{
  using namespace mtb;

  REQUIRE(Wrap(10, 5, 15) == 10);
  REQUIRE(Wrap(15, 5, 15) == 5);
  REQUIRE(Wrap(16, 5, 15) == 6);
  REQUIRE(Wrap(5, 5, 15)  == 5);
  REQUIRE(Wrap(4, 5, 15)  == 14);

  REQUIRE(Wrap(  0, -10, 10) ==   0);
  REQUIRE(Wrap(  1, -10, 10) ==   1);
  REQUIRE(Wrap( 10, -10, 10) == -10);
  REQUIRE(Wrap( 15, -10, 10) ==  -5);
  REQUIRE(Wrap(-15, -10, 10) ==   5);

  REQUIRE(Wrap( 30, 5, 15) == 10);  //  30 => 20 => 10
  REQUIRE(Wrap(-10, 5, 15) == 10);  // -10 =>  0 => 10
}

TEST_CASE("NormalizeValue", "[Common]")
{
  using namespace mtb;

  REQUIRE(NormalizeValue<float>(15, 10, 30) == 0.25f);
  REQUIRE(NormalizeValue<float>(15, 30, 10) == 0.0f); // Invalid bounds.
}

TEST_CASE("AreNearlyEqual", "[Common]")
{
  using namespace mtb;

  REQUIRE( AreNearlyEqual(0.9f, 1.1f,  /* Epsilon: */ 0.200001f));
  REQUIRE(!AreNearlyEqual(0.9f, 1.11f, /* Epsilon: */ 0.2f));
}

namespace
{
  struct SomeDataCounter
  {
    int DefaultConstructions = 0;
    int CopyConstructions = 0;
    int MoveConstructions = 0;
    int Destructions = 0;
  };

  struct SomeData
  {
    static SomeDataCounter* Counter;

    SomeData()                { Counter->DefaultConstructions++; }
    SomeData(SomeData const&) { Counter->CopyConstructions++; }
    SomeData(SomeData&&)      { Counter->MoveConstructions++; }
    ~SomeData()               { Counter->Destructions++; }
  };
  SomeDataCounter* SomeData::Counter = nullptr;

  void TestCopy(SomeData Something) {}
  void TestMove(SomeData&& Something) {}
}

TEST_CASE("Move", "[Common]")
{
  using namespace mtb;

  SomeDataCounter Counter;
  SomeData::Counter = &Counter;
  MTB_Defer [](){ SomeData::Counter = nullptr; };

  SomeData Something;
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 0 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 0 );

  TestCopy(Something);
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 1 );

  TestMove(Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 1 );

  TestCopy(Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 2 );

  TestCopy(SomeData());
  REQUIRE( Counter.DefaultConstructions == 2 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 3 );

  TestMove(Move(SomeData()));
  REQUIRE( Counter.DefaultConstructions == 3 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 4 );
}

struct my_conversion
{
  struct foo {};
  struct bar {};

  static constexpr int
  Do(foo)
  {
    return 42;
  }

  static constexpr int
  Do(bar)
  {
    return 1337;
  }
};

namespace mtb
{
  template<> struct impl_convert<int, my_conversion::foo> : public my_conversion {};
  template<> struct impl_convert<int, my_conversion::bar> : public my_conversion {};
}

TEST_CASE("General Conversion", "[Common]")
{
  using namespace mtb;

  REQUIRE(Convert<int>(my_conversion::foo()) == 42);
  REQUIRE(Convert<int>(my_conversion::bar()) == 1337);
}

TEST_CASE("Float UNorm SNorm Conversion", "[Common]")
{
  using namespace mtb;

  SECTION("u8")
  {
    // Note: We cast to u32 so the output doesn't get treated as a char but
    //       as an integer value.
    REQUIRE( (u32)FloatToUNorm<u8>(0.0f)   == 0 );
    REQUIRE( (u32)FloatToUNorm<u8>(1.0f)   == 255 );
    REQUIRE( (u32)FloatToUNorm<u8>(0.5f)   == 128 );
    REQUIRE( (u32)FloatToUNorm<u8>(0.2f)   == 51 );
    REQUIRE( (u32)FloatToUNorm<u8>(0.25f)  == 64 );

    REQUIRE( UNormToFloat(0) == 0.0f );
    REQUIRE( UNormToFloat(IntMaxValue<u8>()) == 1.0f );
    REQUIRE( AreNearlyEqual(UNormToFloat<u8>(IntMaxValue<u8>() / 2), 0.5f, 0.005f) );

    //
    // Test Clamping
    //
    REQUIRE( (u32)FloatToUNorm<u8>(2.0f)   == 255 );
    REQUIRE( (u32)FloatToUNorm<u8>(-1.0f)  == 0 );
  }
}

TEST_CASE("Sign", "[Common]")
{
  using namespace mtb;

  SECTION("u8")
  {
    REQUIRE( Sign(u8( 42)) ==  1 );
    REQUIRE( Sign(u8( 0)) ==  0 );
  }

  SECTION("u16")
  {
    REQUIRE( Sign(u16( 42)) ==  1 );
    REQUIRE( Sign(u16( 0)) ==  0 );
  }

  SECTION("u32")
  {
    REQUIRE( Sign(u32( 42)) ==  1 );
    REQUIRE( Sign(u32( 0)) ==  0 );
  }

  SECTION("u64")
  {
    REQUIRE( Sign(u64( 42)) ==  1 );
    REQUIRE( Sign(u64( 0)) ==  0 );
  }

  SECTION("i8")
  {
    REQUIRE( Sign(i8(-42)) == -1 );
    REQUIRE( Sign(i8( 42)) ==  1 );
    REQUIRE( Sign(i8( 0)) ==  0 );
  }

  SECTION("i16")
  {
    REQUIRE( Sign(i16(-42)) == -1 );
    REQUIRE( Sign(i16( 42)) ==  1 );
    REQUIRE( Sign(i16( 0)) ==  0 );
  }

  SECTION("i32")
  {
    REQUIRE( Sign(i32(-42)) == -1 );
    REQUIRE( Sign(i32( 42)) ==  1 );
    REQUIRE( Sign(i32( 0)) ==  0 );
  }

  SECTION("i64")
  {
    REQUIRE( Sign(i64(-42)) == -1 );
    REQUIRE( Sign(i64( 42)) ==  1 );
    REQUIRE( Sign(i64( 0)) ==  0 );
  }

  SECTION("float")
  {
    REQUIRE( Sign(-42.1337f) == -1 );
    REQUIRE( Sign( 42.1337f) ==  1 );
    REQUIRE( Sign( 0.00000f) ==  0 );
  }

  SECTION("double")
  {
    REQUIRE( Sign(-42.1337) == -1 );
    REQUIRE( Sign( 42.1337) ==  1 );
    REQUIRE( Sign( 0.00000) ==  0 );
  }
}

TEST_CASE("Abs", "[Common]")
{
  using namespace mtb;

  SECTION("u8")
  {
    REQUIRE( Abs(u8(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<u8>()) == IntMaxValue<u8>() );
  }

  SECTION("u16")
  {
    REQUIRE( Abs(u16(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<u16>()) == IntMaxValue<u16>() );
  }

  SECTION("u32")
  {
    REQUIRE( Abs(u32(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<u32>()) == IntMaxValue<u32>() );
  }

  SECTION("u64")
  {
    REQUIRE( Abs(u64(1)) == 1 );
    REQUIRE( Abs(IntMaxValue<u64>()) == IntMaxValue<u64>() );
  }

  SECTION("i8")
  {
    REQUIRE( Abs(i8(1)) == 1 );
    REQUIRE( Abs(i8(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<i8>()) == IntMaxValue<i8>() );
    REQUIRE( Abs(IntMinValue<i8>() + 1) == IntMaxValue<i8>() );
  }

  SECTION("i16")
  {
    REQUIRE( Abs(i16(1)) == 1 );
    REQUIRE( Abs(i16(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<i16>()) == IntMaxValue<i16>() );
    REQUIRE( Abs(IntMinValue<i16>() + 1) == IntMaxValue<i16>() );
  }

  SECTION("i32")
  {
    REQUIRE( Abs(i32(1)) == 1 );
    REQUIRE( Abs(i32(-1)) == 1 );
    REQUIRE( Abs(IntMaxValue<i32>()) == IntMaxValue<i32>() );
    REQUIRE( Abs(IntMinValue<i32>() + 1) == IntMaxValue<i32>() );
  }

  SECTION("i64")
  {
    REQUIRE( Abs(i64(1)) == 1 );
    REQUIRE( Abs(i64(-1)) == 1 );
    REQUIRE( Abs(IntMinValue<i64>() + 1) == IntMaxValue<i64>() );
    REQUIRE( Abs(IntMaxValue<i64>()) == IntMaxValue<i64>() );
  }

  SECTION("float")
  {
    REQUIRE( Abs( 1.0f) == 1.0f );
    REQUIRE( Abs(-1.0f) == 1.0f );
  }

  SECTION("double")
  {
    REQUIRE( Abs( 1.0) == 1.0 );
    REQUIRE( Abs(-1.0) == 1.0 );
  }
}


TEST_CASE("Rounding", "[Common]")
{
  using namespace mtb;

  SECTION("RoundDown")
  {
    REQUIRE( RoundDown<int>( 3.40f) ==  3 );
    REQUIRE( RoundDown<int>( 3.50f) ==  3 );
    REQUIRE( RoundDown<int>( 3.60f) ==  3 );
    REQUIRE( RoundDown<int>(-3.40f) == -4 );
    REQUIRE( RoundDown<int>(-3.50f) == -4 );
    REQUIRE( RoundDown<int>(-3.60f) == -4 );
  }

  SECTION("RoundUp")
  {
    REQUIRE( RoundUp<int>( 3.40f) ==  4 );
    REQUIRE( RoundUp<int>( 3.50f) ==  4 );
    REQUIRE( RoundUp<int>( 3.60f) ==  4 );
    REQUIRE( RoundUp<int>(-3.40f) == -3 );
    REQUIRE( RoundUp<int>(-3.50f) == -3 );
    REQUIRE( RoundUp<int>(-3.60f) == -3 );
  }

  SECTION("RoundTowardsZero")
  {
    REQUIRE( RoundTowardsZero<int>( 3.40f) ==  3 );
    REQUIRE( RoundTowardsZero<int>( 3.50f) ==  3 );
    REQUIRE( RoundTowardsZero<int>( 3.60f) ==  3 );
    REQUIRE( RoundTowardsZero<int>(-3.40f) == -3 );
    REQUIRE( RoundTowardsZero<int>(-3.50f) == -3 );
    REQUIRE( RoundTowardsZero<int>(-3.60f) == -3 );
  }

  SECTION("RoundAwayFromZero")
  {
    REQUIRE( RoundAwayFromZero<int>( 3.40f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>( 3.50f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>( 3.60f) ==  4 );
    REQUIRE( RoundAwayFromZero<int>(-3.40f) == -4 );
    REQUIRE( RoundAwayFromZero<int>(-3.50f) == -4 );
    REQUIRE( RoundAwayFromZero<int>(-3.60f) == -4 );
  }

  SECTION("Round")
  {
    REQUIRE( Round<int>( 3.40f) ==  3 );
    REQUIRE( Round<int>( 3.50f) ==  4 );
    REQUIRE( Round<int>( 3.60f) ==  4 );
    REQUIRE( Round<int>(-3.40f) == -3 );
    REQUIRE( Round<int>(-3.50f) == -3 );
    REQUIRE( Round<int>(-3.51f) == -4 );
    REQUIRE( Round<int>(-3.60f) == -4 );
  }
}
