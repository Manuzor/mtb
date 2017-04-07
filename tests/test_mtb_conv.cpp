#include "mtb_conv.hpp"


static void
TestFloat(char const* String, double ExpectedValue, int ExpectedRemainingSourceLen)
{
  CAPTURE( String );
  CAPTURE( ExpectedValue );
  CAPTURE( ExpectedRemainingSourceLen );

  size_t Len = mtb_StringLengthOf(String);
  mtb_parse_string_result_f64 Result = mtb_ParseString_f64(Len, String);
  CAPTURE( Result.Value );
  REQUIRE( Result.Success );
  REQUIRE( mtb_AreNearlyEqual(Result.Value, ExpectedValue, 1e-10) );
  REQUIRE( Result.RemainingSourceLen == ExpectedRemainingSourceLen );
}

TEST_CASE("String Conversion: String -> Floating Point", "[StringConversion]")
{
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

  char const* SourcePtr = "ABC";
  mtb_parse_string_result_f64 Result = mtb_ParseString_f64(3, SourcePtr);
  REQUIRE( !Result.Success );
  REQUIRE( mtb_IsNaN(Result.Value) );
  REQUIRE( Result.RemainingSourceLen == 3 );
  REQUIRE( Result.RemainingSourcePtr == SourcePtr );
}

static void
TestInt(char const* String, mtb_s64 ExpectedValue, int ExpectedRemainingSourceLen)
{
  CAPTURE( String );
  CAPTURE( ExpectedValue );
  CAPTURE( ExpectedRemainingSourceLen );

  size_t Len = mtb_StringLengthOf(String);
  mtb_parse_string_result_s64 Result = mtb_ParseString_s64(Len, String, -1);
  CAPTURE( Result.Value );
  REQUIRE( Result.Success );
  REQUIRE( Result.Value == ExpectedValue );
  REQUIRE( Result.RemainingSourceLen == ExpectedRemainingSourceLen );
}

TEST_CASE("String Conversion: String -> Integer", "[StringConversion]")
{
  TestInt("1", 1, 0);
  TestInt("-1", -1, 0);
  TestInt("400000000", 400000000, 0);
  TestInt("-400000000", -400000000, 0);
  TestInt("23443A", 23443, 1);
  TestInt("  23443A", 23443, 1);
  TestInt("\n \r  \t23443A", 23443, 1);
  TestInt("76.55.43", 76, 6);

  char const* SourcePtr = "ABC";
  mtb_parse_string_result_s64 Result = mtb_ParseString_s64(3, SourcePtr, -1);
  REQUIRE( !Result.Success );
  REQUIRE( Result.Value == -1 );
  REQUIRE( Result.RemainingSourceLen == 3 );
  REQUIRE( Result.RemainingSourcePtr == SourcePtr );
}

TEST_CASE("String Conversion: Integer -> String", "[StringConversion]")
{
  char Buffer[32]{};
  mtb_to_string_result Result;

  Result = mtb_ToString(42, 32, Buffer);
  CAPTURE( Result );
  REQUIRE( Result.Success );
  REQUIRE( mtb_StringsAreEqual(Result.StrLen, Result.StrPtr, "42") );

  Result = mtb_ToString(0, 32, Buffer);
  CAPTURE( Result );
  REQUIRE( Result.Success );
  REQUIRE( mtb_StringsAreEqual(Result.StrLen, Result.StrPtr, "0") );
}

