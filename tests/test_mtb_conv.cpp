#include "mtb_conv.hpp"


static void
TestFloat(char const* String, double ExpectedValue, int ExpectedRemainingNumStringElements)
{
  using namespace mtb;

  CAPTURE( String );
  CAPTURE( ExpectedValue );
  CAPTURE( ExpectedRemainingNumStringElements );

  auto Source = SliceFromString(String);
  convert_string_to_number_result<double> Result = Convert<double>(Source);
  CAPTURE( Result.Value );
  REQUIRE( Result.Success );
  REQUIRE( AreNearlyEqual(Result.Value, ExpectedValue, 1e-10) );
  REQUIRE( LengthOf(Result.RemainingSource) == ExpectedRemainingNumStringElements );
}

TEST_CASE("String Conversion: String -> Floating Point", "[StringConversion]")
{
  using namespace mtb;

  TestFloat("1", 1.0f, 0);
  TestFloat("-1", -1.0f, 0);
  TestFloat("1.5", 1.5f, 0);
  TestFloat("-1.5", -1.5f, 0);
  TestFloat("1E10", 1e10f, 0);
  TestFloat("1E-10", 1e-10f, 0);
  TestFloat("-1E-10", -1e-10f, 0);
  TestFloat("-1E10", -1e10f, 0);
  TestFloat("1.234E-10", 1.234e-10f, 0);
  TestFloat("-1.234E-10", -1.234e-10f, 0);

  TestFloat("1e10", 1e10f, 0);
  TestFloat("1e-10", 1e-10f, 0);
  TestFloat("-1e-10", -1e-10f, 0);
  TestFloat("-1e10", -1e10f, 0);
  TestFloat("1.234e-10", 1.234e-10f, 0);
  TestFloat("-1.234e-10", -1.234e-10f, 0);

  TestFloat("23443A", 23443, 1);
  TestFloat("  23443A", 23443, 1);
  TestFloat("\n \r  \t23443A", 23443, 1);
  TestFloat("76.55.43", 76.55, 3);
  TestFloat(".3", 0.3, 0);

  auto Source = SliceFromString("ABC");
  auto Result = Convert<double>(Source);
  REQUIRE( !Result.Success );
  REQUIRE( IsNaN(Result.Value) );
  REQUIRE( LengthOf(Result.RemainingSource) == 3 );
}

static void
TestInt(char const* String, mtb::i64 ExpectedValue, int ExpectedRemainingNumStringElements)
{
  using namespace mtb;

  CAPTURE( String );
  CAPTURE( ExpectedValue );
  CAPTURE( ExpectedRemainingNumStringElements );

  slice<char const> Source = SliceFromString(String);
  convert_string_to_number_result<i64> Result = Convert<i64>(Source);
  CAPTURE( Result.Value );
  REQUIRE( Result.Success );
  REQUIRE( Result.Value == ExpectedValue );
  REQUIRE( LengthOf(Result.RemainingSource) == ExpectedRemainingNumStringElements );
}

TEST_CASE("String Conversion: String -> Integer", "[StringConversion]")
{
  using namespace mtb;

  TestInt("1", 1, 0);
  TestInt("-1", -1, 0);
  TestInt("400000000", 400000000, 0);
  TestInt("-400000000", -400000000, 0);
  TestInt("23443A", 23443, 1);
  TestInt("  23443A", 23443, 1);
  TestInt("\n \r  \t23443A", 23443, 1);
  TestInt("76.55.43", 76, 6);

  auto Source = SliceFromString("ABC");
  auto Result = Convert<i64>(Source);
  REQUIRE( !Result.Success );
  REQUIRE( Result.Value == 0 );
  REQUIRE( LengthOf(Result.RemainingSource) == 3 );
}

TEST_CASE("String Conversion: Integer -> String", "[StringConversion]")
{
  using namespace mtb;

  char FixedBuffer[32]{};
  auto Buffer = Slice(FixedBuffer);
  convert_number_to_string_result Result;

  Result = Convert<slice<char>>((int)42, Buffer);
  REQUIRE( Result.Success );
  REQUIRE( Result.Value == "42"_S );

  Result = Convert<slice<char>>((int)0, Buffer);
  REQUIRE( Result.Success );
  REQUIRE( Result.Value == "0"_S );
}
