#include "mtb_common.h"


static_assert(MTB_IsPod(void), "Type 'void' must be considered POD.");
static_assert(MTB_IsPod(void const), "Type 'void const' must be considered POD.");

static_assert(mtb_IsNaN(MTB_NaN_f32), "NaN stuff not working properly.");
static_assert(mtb_IsNaN(MTB_NaN_f64), "NaN stuff not working properly.");


TEST_CASE("MTB_DEFER", "[Common]")
{
  SECTION("POD")
  {
    int Value = 0;

    MTB_DEFER [&](){ REQUIRE(Value == 3); };
    MTB_DEFER [&](){ ++Value; };
    MTB_DEFER [&](){ ++Value; };
    MTB_DEFER [=](){ REQUIRE( Value == 0 ); };
    MTB_DEFER [&](){ ++Value; };
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

    MTB_DEFER [&](){ Foo.Value = 42; Bar = {}; };
    MTB_DEFER [Bar](){ REQUIRE( Bar.Value == 1337 ); };
    MTB_DEFER [&Bar](){ REQUIRE( Bar.Value == 123 ); };
    MTB_DEFER [=, &Foo](){ REQUIRE( Foo.Value == 3 ); REQUIRE( Bar.Value == 1337 ); };

    Foo.Value = 3;

    Bar.Value = 123;
  }
}

TEST_CASE("Byte Sizes", "[Common]")
{
  REQUIRE( mtb_KiB(3) == 3 * 1024ULL );
  REQUIRE( mtb_MiB(3) == 3 * 1024ULL * 1024ULL );
  REQUIRE( mtb_GiB(3) == 3 * 1024ULL * 1024ULL * 1024ULL );
  REQUIRE( mtb_TiB(3) == 3 * 1024ULL * 1024ULL * 1024ULL * 1024ULL );

  REQUIRE( mtb_KB(3) == 3 * 1000ULL );
  REQUIRE( mtb_MB(3) == 3 * 1000ULL * 1000ULL );
  REQUIRE( mtb_GB(3) == 3 * 1000ULL * 1000ULL * 1000ULL );
  REQUIRE( mtb_TB(3) == 3 * 1000ULL * 1000ULL * 1000ULL * 1000ULL );

  REQUIRE( mtb_ToKiB(512) == 0.5f );
  REQUIRE( mtb_ToKiB(2 * 1024) == 2.0f );
}

TEST_CASE("Length of static arrays", "[Common]")
{
  SECTION("POD array")
  {
    int Array[42];
    REQUIRE(mtb_ArrayLengthOf(Array) == 42);
  }

  SECTION("String literal")
  {
    REQUIRE(mtb_ArrayLengthOf("Foo") == 3 + 1);
  }
}

TEST_CASE("mtb_IsPowerOfTwo", "[Common]")
{
  for(size_t Index = 0; Index < MTB_GetNumBits(size_t); ++Index)
  {
    CAPTURE( Index );
    auto Integer = size_t(1) << Index;
    CAPTURE( Integer );
    REQUIRE( mtb_IsPowerOfTwo(Integer) );
  }

  auto NegativeTest = [](size_t Argument)
  {
    CAPTURE(Argument);
    REQUIRE( !mtb_IsPowerOfTwo(Argument) );
  };

  NegativeTest( 3 );
  NegativeTest( 5 );
  NegativeTest( 6 );
  NegativeTest( 12 );
  NegativeTest( 1024-1 );
}

TEST_CASE("mtb_Min", "[Common]")
{
  REQUIRE(mtb_Min( 0,  1) == 0);
  REQUIRE(mtb_Min( 1,  0) == 0);
  REQUIRE(mtb_Min(-1,  0) == -1);
  REQUIRE(mtb_Min( 0, -1) == -1);
}

TEST_CASE("mtb_Max", "[Common]")
{
  REQUIRE(mtb_Max( 0,  1) == 1);
  REQUIRE(mtb_Max( 1,  0) == 1);
  REQUIRE(mtb_Max(-1,  0) == 0);
  REQUIRE(mtb_Max( 0, -1) == 0);
}

TEST_CASE("mtb_Clamp", "[Common]")
{
  REQUIRE(mtb_Clamp(10, 5, 15) == 10);
  REQUIRE(mtb_Clamp( 5, 5, 15) ==  5);
  REQUIRE(mtb_Clamp(15, 5, 15) == 15);
  REQUIRE(mtb_Clamp(20, 5, 15) == 15);
  REQUIRE(mtb_Clamp( 0, 5, 15) ==  5);

  REQUIRE(mtb_Clamp(10, 15, 5) ==  10);
}

TEST_CASE("mtb_NormalizeValue", "[Common]")
{
  REQUIRE(mtb_NormalizeValue<mtb_f32>(15, 10, 30) == 0.25f);
  REQUIRE(mtb_NormalizeValue<mtb_f32>(15, 30, 10) == 0.0f); // Invalid bounds.
}

TEST_CASE("mtb_AreNearlyEqual", "[Common]")
{
  REQUIRE( mtb_AreNearlyEqual(0.9f, 1.1f,  /* Epsilon: */ 0.200001f));
  REQUIRE(!mtb_AreNearlyEqual(0.9f, 1.11f, /* Epsilon: */ 0.2f));
}

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

TEST_CASE("mtb_Move", "[Common]")
{
  SomeDataCounter Counter;
  SomeData::Counter = &Counter;
  MTB_DEFER [](){ SomeData::Counter = nullptr; };

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

  TestMove(mtb_Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 0 );
  REQUIRE( Counter.Destructions         == 1 );

  TestCopy(mtb_Move(Something));
  REQUIRE( Counter.DefaultConstructions == 1 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 2 );

  TestCopy(SomeData());
  REQUIRE( Counter.DefaultConstructions == 2 );
  REQUIRE( Counter.CopyConstructions    == 1 );
  REQUIRE( Counter.MoveConstructions    == 1 );
  REQUIRE( Counter.Destructions         == 3 );

  TestMove(mtb_Move(SomeData()));
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

template<> struct mtb_impl_convert<int, my_conversion::foo> : public my_conversion {};
template<> struct mtb_impl_convert<int, my_conversion::bar> : public my_conversion {};

TEST_CASE("General Conversion", "[Common]")
{
  REQUIRE(mtb_Convert<int>(my_conversion::foo()) == 42);
  REQUIRE(mtb_Convert<int>(my_conversion::bar()) == 1337);
}

TEST_CASE("Float UNorm SNorm Conversion", "[Common]")
{
  SECTION("mtb_u08")
  {
    // Note: We cast to mtb_u32 so the output doesn't get treated as a char but
    //       as an integer value.
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(0.0f)  == 0 );
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(1.0f)  == 255 );
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(0.5f)  == 128 );
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(0.2f)  == 51 );
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(0.25f) == 64 );

    REQUIRE( mtb_Normalized_u08_To_f32(0) == 0.0f );
    REQUIRE( mtb_Normalized_u08_To_f32(MTB_MaxValue_u08) == 1.0f );
    REQUIRE( mtb_AreNearlyEqual(mtb_Normalized_u08_To_f32(MTB_MaxValue_u08 / 2), 0.5f, 0.005f) );

    //
    // Test Clamping
    //
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(2.0f)  == 255 );
    REQUIRE( (mtb_u32)mtb_Normalized_f32_To_u08(-1.0f) == 0 );
  }
}

TEST_CASE("mtb_Sign", "[Common]")
{
  SECTION("mtb_s08")
  {
    REQUIRE( mtb_Sign((mtb_s08)-42) == -1 );
    REQUIRE( mtb_Sign((mtb_s08) 42) ==  1 );
    REQUIRE( mtb_Sign((mtb_s08)  0) ==  0 );
  }

  SECTION("mtb_s16")
  {
    REQUIRE( mtb_Sign((mtb_s16)-42) == -1 );
    REQUIRE( mtb_Sign((mtb_s16) 42) ==  1 );
    REQUIRE( mtb_Sign((mtb_s16)  0) ==  0 );
  }

  SECTION("mtb_s32")
  {
    REQUIRE( mtb_Sign((mtb_s32)-42) == -1 );
    REQUIRE( mtb_Sign((mtb_s32) 42) ==  1 );
    REQUIRE( mtb_Sign((mtb_s32)  0) ==  0 );
  }

  SECTION("mtb_s64")
  {
    REQUIRE( mtb_Sign((mtb_s64)-42) == -1 );
    REQUIRE( mtb_Sign((mtb_s64) 42) ==  1 );
    REQUIRE( mtb_Sign((mtb_s64)  0) ==  0 );
  }

  SECTION("mtb_f32")
  {
    REQUIRE( mtb_Sign(-42.1337f) == -1 );
    REQUIRE( mtb_Sign( 42.1337f) ==  1 );
    REQUIRE( mtb_Sign( 0.00000f) ==  0 );
  }

  SECTION("mtb_f64")
  {
    REQUIRE( mtb_Sign(-42.1337) == -1 );
    REQUIRE( mtb_Sign( 42.1337) ==  1 );
    REQUIRE( mtb_Sign( 0.00000) ==  0 );
  }
}

TEST_CASE("mtb_Abs", "[Common]")
{
  SECTION("mtb_s08")
  {
    REQUIRE( (mtb_s08)mtb_Abs(mtb_s08( 1)) == 1 );
    REQUIRE( (mtb_s08)mtb_Abs(mtb_s08(-1)) == 1 );
    REQUIRE( (mtb_s08)mtb_Abs(MTB_MaxValue_s08) == MTB_MaxValue_s08 );
    REQUIRE( (mtb_s08)mtb_Abs(MTB_MinValue_s08 + 1) == MTB_MaxValue_s08 );
  }

  SECTION("mtb_s16")
  {
    REQUIRE( (mtb_s16)mtb_Abs(mtb_s16( 1)) == 1 );
    REQUIRE( (mtb_s16)mtb_Abs(mtb_s16(-1)) == 1 );
    REQUIRE( (mtb_s16)mtb_Abs(MTB_MaxValue_s16) == MTB_MaxValue_s16 );
    REQUIRE( (mtb_s16)mtb_Abs(MTB_MinValue_s16 + 1) == MTB_MaxValue_s16 );
  }

  SECTION("mtb_s32")
  {
    REQUIRE( (mtb_s32)mtb_Abs(mtb_s32( 1)) == 1 );
    REQUIRE( (mtb_s32)mtb_Abs(mtb_s32(-1)) == 1 );
    REQUIRE( (mtb_s32)mtb_Abs(MTB_MaxValue_s32) == MTB_MaxValue_s32 );
    REQUIRE( (mtb_s32)mtb_Abs(MTB_MinValue_s32 + 1) == MTB_MaxValue_s32 );
  }

  SECTION("mtb_s64")
  {
    REQUIRE( (mtb_s64)mtb_Abs(mtb_s64( 1)) == 1 );
    REQUIRE( (mtb_s64)mtb_Abs(mtb_s64(-1)) == 1 );
    REQUIRE( (mtb_s64)mtb_Abs(MTB_MinValue_s64 + 1) == MTB_MaxValue_s64 );
    REQUIRE( (mtb_s64)mtb_Abs(MTB_MaxValue_s64) == MTB_MaxValue_s64 );
  }

  SECTION("mtb_f32")
  {
    REQUIRE( mtb_Abs( 1.0f) == 1.0f );
    REQUIRE( mtb_Abs(-1.0f) == 1.0f );
  }

  SECTION("mtb_f64")
  {
    REQUIRE( mtb_Abs( 1.0) == 1.0 );
    REQUIRE( mtb_Abs(-1.0) == 1.0 );
  }
}


TEST_CASE("Rounding", "[Common]")
{
  SECTION("RoundDown")
  {
    REQUIRE( (int)mtb_RoundDown( 3.40f) ==  3 );
    REQUIRE( (int)mtb_RoundDown( 3.50f) ==  3 );
    REQUIRE( (int)mtb_RoundDown( 3.60f) ==  3 );
    REQUIRE( (int)mtb_RoundDown(-3.40f) == -4 );
    REQUIRE( (int)mtb_RoundDown(-3.50f) == -4 );
    REQUIRE( (int)mtb_RoundDown(-3.60f) == -4 );
  }

  SECTION("RoundUp")
  {
    REQUIRE( (int)mtb_RoundUp( 3.40f) ==  4 );
    REQUIRE( (int)mtb_RoundUp( 3.50f) ==  4 );
    REQUIRE( (int)mtb_RoundUp( 3.60f) ==  4 );
    REQUIRE( (int)mtb_RoundUp(-3.40f) == -3 );
    REQUIRE( (int)mtb_RoundUp(-3.50f) == -3 );
    REQUIRE( (int)mtb_RoundUp(-3.60f) == -3 );
  }

  SECTION("RoundTowardsZero")
  {
    REQUIRE( (int)mtb_RoundTowardsZero( 3.40f) ==  3 );
    REQUIRE( (int)mtb_RoundTowardsZero( 3.50f) ==  3 );
    REQUIRE( (int)mtb_RoundTowardsZero( 3.60f) ==  3 );
    REQUIRE( (int)mtb_RoundTowardsZero(-3.40f) == -3 );
    REQUIRE( (int)mtb_RoundTowardsZero(-3.50f) == -3 );
    REQUIRE( (int)mtb_RoundTowardsZero(-3.60f) == -3 );
  }

  SECTION("RoundAwayFromZero")
  {
    REQUIRE( (int)mtb_RoundAwayFromZero( 3.40f) ==  4 );
    REQUIRE( (int)mtb_RoundAwayFromZero( 3.50f) ==  4 );
    REQUIRE( (int)mtb_RoundAwayFromZero( 3.60f) ==  4 );
    REQUIRE( (int)mtb_RoundAwayFromZero(-3.40f) == -4 );
    REQUIRE( (int)mtb_RoundAwayFromZero(-3.50f) == -4 );
    REQUIRE( (int)mtb_RoundAwayFromZero(-3.60f) == -4 );
  }

  SECTION("Round")
  {
    REQUIRE( (int)mtb_Round( 3.40f) ==  3 );
    REQUIRE( (int)mtb_Round( 3.50f) ==  4 );
    REQUIRE( (int)mtb_Round( 3.60f) ==  4 );
    REQUIRE( (int)mtb_Round(-3.40f) == -3 );
    REQUIRE( (int)mtb_Round(-3.50f) == -3 );
    REQUIRE( (int)mtb_Round(-3.51f) == -4 );
    REQUIRE( (int)mtb_Round(-3.60f) == -4 );
  }
}

TEST_CASE("Concat Strings", "[Common]")
{
  char Buffer[128];
  mtb_concat_strings_result Result = mtb_ConcatStrings("ABC", "DEF", mtb_ArrayLengthOf(Buffer), Buffer);
  CAPTURE( Result.Len );
  CAPTURE( Result.Ptr );
  REQUIRE( mtb_StringsAreEqual("ABCDEF", Result.Ptr) );
}
