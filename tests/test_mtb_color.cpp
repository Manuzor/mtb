#include "mtb_color.h"


TEST_CASE("Color Conversion", "[Color]")
{
  SECTION("Gamma UB => Linear Float")
  {
    mtb_color const C1 = mtb_Color(0x64, 0x95, 0xED); // Cornflower Blue
    mtb_linear_color const C2 = mtb_ToLinearColor(C1);

    REQUIRE( mtb_AreNearlyEqual(C2.R, 0.12744f) );
    REQUIRE( mtb_AreNearlyEqual(C2.G, 0.30054f) );
    REQUIRE( mtb_AreNearlyEqual(C2.B, 0.84687f) );
    REQUIRE( C2.A == 1.0f );
  }
}

// More tests.

TEST_CASE("Pre-defined color values", "[Color]")
{
  SECTION( "AliceBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_AliceBlue));
    REQUIRE( Color.R == 0xF0 );
    REQUIRE( Color.G == 0xF8 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "AntiqueWhite" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_AntiqueWhite));
    REQUIRE( Color.R == 0xFA );
    REQUIRE( Color.G == 0xEB );
    REQUIRE( Color.B == 0xD7 );
  }
  SECTION( "Aqua" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Aqua));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "Aquamarine" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Aquamarine));
    REQUIRE( Color.R == 0x7F );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xD4 );
  }
  SECTION( "Azure" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Azure));
    REQUIRE( Color.R == 0xF0 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "Beige" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Beige));
    REQUIRE( Color.R == 0xF5 );
    REQUIRE( Color.G == 0xF5 );
    REQUIRE( Color.B == 0xDC );
  }
  SECTION( "Bisque" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Bisque));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xE4 );
    REQUIRE( Color.B == 0xC4 );
  }
  SECTION( "Black" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Black));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "BlanchedAlmond" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_BlanchedAlmond));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xEB );
    REQUIRE( Color.B == 0xCD );
  }
  SECTION( "Blue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Blue));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "BlueViolet" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_BlueViolet));
    REQUIRE( Color.R == 0x8A );
    REQUIRE( Color.G == 0x2B );
    REQUIRE( Color.B == 0xE2 );
  }
  SECTION( "Brown" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Brown));
    REQUIRE( Color.R == 0xA5 );
    REQUIRE( Color.G == 0x2A );
    REQUIRE( Color.B == 0x2A );
  }
  SECTION( "BurlyWood" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_BurlyWood));
    REQUIRE( Color.R == 0xDE );
    REQUIRE( Color.G == 0xB8 );
    REQUIRE( Color.B == 0x87 );
  }
  SECTION( "CadetBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_CadetBlue));
    REQUIRE( Color.R == 0x5F );
    REQUIRE( Color.G == 0x9E );
    REQUIRE( Color.B == 0xA0 );
  }
  SECTION( "Chartreuse" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Chartreuse));
    REQUIRE( Color.R == 0x7F );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "Chocolate" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Chocolate));
    REQUIRE( Color.R == 0xD2 );
    REQUIRE( Color.G == 0x69 );
    REQUIRE( Color.B == 0x1E );
  }
  SECTION( "Coral" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Coral));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x7F );
    REQUIRE( Color.B == 0x50 );
  }
  SECTION( "CornflowerBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_CornflowerBlue));
    REQUIRE( Color.R == 0x64 );
    REQUIRE( Color.G == 0x95 );
    REQUIRE( Color.B == 0xED );
  }
  SECTION( "Cornsilk" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Cornsilk));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xF8 );
    REQUIRE( Color.B == 0xDC );
  }
  SECTION( "Crimson" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Crimson));
    REQUIRE( Color.R == 0xDC );
    REQUIRE( Color.G == 0x14 );
    REQUIRE( Color.B == 0x3C );
  }
  SECTION( "Cyan" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Cyan));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "DarkBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkBlue));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x8B );
  }
  SECTION( "DarkCyan" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkCyan));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x8B );
    REQUIRE( Color.B == 0x8B );
  }
  SECTION( "DarkGoldenRod" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkGoldenRod));
    REQUIRE( Color.R == 0xB8 );
    REQUIRE( Color.G == 0x86 );
    REQUIRE( Color.B == 0x0B );
  }
  SECTION( "DarkGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkGray));
    REQUIRE( Color.R == 0xA9 );
    REQUIRE( Color.G == 0xA9 );
    REQUIRE( Color.B == 0xA9 );
  }
  SECTION( "DarkGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkGreen));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x64 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "DarkKhaki" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkKhaki));
    REQUIRE( Color.R == 0xBD );
    REQUIRE( Color.G == 0xB7 );
    REQUIRE( Color.B == 0x6B );
  }
  SECTION( "DarkMagenta" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkMagenta));
    REQUIRE( Color.R == 0x8B );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x8B );
  }
  SECTION( "DarkOliveGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkOliveGreen));
    REQUIRE( Color.R == 0x55 );
    REQUIRE( Color.G == 0x6B );
    REQUIRE( Color.B == 0x2F );
  }
  SECTION( "DarkOrange" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkOrange));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x8C );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "DarkOrchid" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkOrchid));
    REQUIRE( Color.R == 0x99 );
    REQUIRE( Color.G == 0x32 );
    REQUIRE( Color.B == 0xCC );
  }
  SECTION( "DarkRed" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkRed));
    REQUIRE( Color.R == 0x8B );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "DarkSalmon" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkSalmon));
    REQUIRE( Color.R == 0xE9 );
    REQUIRE( Color.G == 0x96 );
    REQUIRE( Color.B == 0x7A );
  }
  SECTION( "DarkSeaGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkSeaGreen));
    REQUIRE( Color.R == 0x8F );
    REQUIRE( Color.G == 0xBC );
    REQUIRE( Color.B == 0x8F );
  }
  SECTION( "DarkSlateBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkSlateBlue));
    REQUIRE( Color.R == 0x48 );
    REQUIRE( Color.G == 0x3D );
    REQUIRE( Color.B == 0x8B );
  }
  SECTION( "DarkSlateGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkSlateGray));
    REQUIRE( Color.R == 0x2F );
    REQUIRE( Color.G == 0x4F );
    REQUIRE( Color.B == 0x4F );
  }
  SECTION( "DarkTurquoise" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkTurquoise));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xCE );
    REQUIRE( Color.B == 0xD1 );
  }
  SECTION( "DarkViolet" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DarkViolet));
    REQUIRE( Color.R == 0x94 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0xD3 );
  }
  SECTION( "DeepPink" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DeepPink));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x14 );
    REQUIRE( Color.B == 0x93 );
  }
  SECTION( "DeepSkyBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DeepSkyBlue));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xBF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "DimGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DimGray));
    REQUIRE( Color.R == 0x69 );
    REQUIRE( Color.G == 0x69 );
    REQUIRE( Color.B == 0x69 );
  }
  SECTION( "DodgerBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_DodgerBlue));
    REQUIRE( Color.R == 0x1E );
    REQUIRE( Color.G == 0x90 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "FireBrick" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_FireBrick));
    REQUIRE( Color.R == 0xB2 );
    REQUIRE( Color.G == 0x22 );
    REQUIRE( Color.B == 0x22 );
  }
  SECTION( "FloralWhite" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_FloralWhite));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFA );
    REQUIRE( Color.B == 0xF0 );
  }
  SECTION( "ForestGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_ForestGreen));
    REQUIRE( Color.R == 0x22 );
    REQUIRE( Color.G == 0x8B );
    REQUIRE( Color.B == 0x22 );
  }
  SECTION( "Fuchsia" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Fuchsia));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "Gainsboro" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Gainsboro));
    REQUIRE( Color.R == 0xDC );
    REQUIRE( Color.G == 0xDC );
    REQUIRE( Color.B == 0xDC );
  }
  SECTION( "GhostWhite" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_GhostWhite));
    REQUIRE( Color.R == 0xF8 );
    REQUIRE( Color.G == 0xF8 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "Gold" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Gold));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xD7 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "GoldenRod" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_GoldenRod));
    REQUIRE( Color.R == 0xDA );
    REQUIRE( Color.G == 0xA5 );
    REQUIRE( Color.B == 0x20 );
  }
  SECTION( "Gray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Gray));
    REQUIRE( Color.R == 0x80 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x80 );
  }
  SECTION( "Green" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Green));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "GreenYellow" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_GreenYellow));
    REQUIRE( Color.R == 0xAD );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0x2F );
  }
  SECTION( "HoneyDew" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_HoneyDew));
    REQUIRE( Color.R == 0xF0 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xF0 );
  }
  SECTION( "HotPink" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_HotPink));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x69 );
    REQUIRE( Color.B == 0xB4 );
  }
  SECTION( "IndianRed" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_IndianRed));
    REQUIRE( Color.R == 0xCD );
    REQUIRE( Color.G == 0x5C );
    REQUIRE( Color.B == 0x5C );
  }
  SECTION( "Indigo" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Indigo));
    REQUIRE( Color.R == 0x4B );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x82 );
  }
  SECTION( "Ivory" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Ivory));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xF0 );
  }
  SECTION( "Khaki" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Khaki));
    REQUIRE( Color.R == 0xF0 );
    REQUIRE( Color.G == 0xE6 );
    REQUIRE( Color.B == 0x8C );
  }
  SECTION( "Lavender" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Lavender));
    REQUIRE( Color.R == 0xE6 );
    REQUIRE( Color.G == 0xE6 );
    REQUIRE( Color.B == 0xFA );
  }
  SECTION( "LavenderBlush" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LavenderBlush));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xF0 );
    REQUIRE( Color.B == 0xF5 );
  }
  SECTION( "LawnGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LawnGreen));
    REQUIRE( Color.R == 0x7C );
    REQUIRE( Color.G == 0xFC );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "LemonChiffon" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LemonChiffon));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFA );
    REQUIRE( Color.B == 0xCD );
  }
  SECTION( "LightBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightBlue));
    REQUIRE( Color.R == 0xAD );
    REQUIRE( Color.G == 0xD8 );
    REQUIRE( Color.B == 0xE6 );
  }
  SECTION( "LightCoral" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightCoral));
    REQUIRE( Color.R == 0xF0 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x80 );
  }
  SECTION( "LightCyan" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightCyan));
    REQUIRE( Color.R == 0xE0 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "LightGoldenRodYellow" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightGoldenRodYellow));
    REQUIRE( Color.R == 0xFA );
    REQUIRE( Color.G == 0xFA );
    REQUIRE( Color.B == 0xD2 );
  }
  SECTION( "LightGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightGray));
    REQUIRE( Color.R == 0xD3 );
    REQUIRE( Color.G == 0xD3 );
    REQUIRE( Color.B == 0xD3 );
  }
  SECTION( "LightGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightGreen));
    REQUIRE( Color.R == 0x90 );
    REQUIRE( Color.G == 0xEE );
    REQUIRE( Color.B == 0x90 );
  }
  SECTION( "LightPink" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightPink));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xB6 );
    REQUIRE( Color.B == 0xC1 );
  }
  SECTION( "LightSalmon" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightSalmon));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xA0 );
    REQUIRE( Color.B == 0x7A );
  }
  SECTION( "LightSeaGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightSeaGreen));
    REQUIRE( Color.R == 0x20 );
    REQUIRE( Color.G == 0xB2 );
    REQUIRE( Color.B == 0xAA );
  }
  SECTION( "LightSkyBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightSkyBlue));
    REQUIRE( Color.R == 0x87 );
    REQUIRE( Color.G == 0xCE );
    REQUIRE( Color.B == 0xFA );
  }
  SECTION( "LightSlateGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightSlateGray));
    REQUIRE( Color.R == 0x77 );
    REQUIRE( Color.G == 0x88 );
    REQUIRE( Color.B == 0x99 );
  }
  SECTION( "LightSteelBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightSteelBlue));
    REQUIRE( Color.R == 0xB0 );
    REQUIRE( Color.G == 0xC4 );
    REQUIRE( Color.B == 0xDE );
  }
  SECTION( "LightYellow" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LightYellow));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xE0 );
  }
  SECTION( "Lime" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Lime));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "LimeGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_LimeGreen));
    REQUIRE( Color.R == 0x32 );
    REQUIRE( Color.G == 0xCD );
    REQUIRE( Color.B == 0x32 );
  }
  SECTION( "Linen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Linen));
    REQUIRE( Color.R == 0xFA );
    REQUIRE( Color.G == 0xF0 );
    REQUIRE( Color.B == 0xE6 );
  }
  SECTION( "Magenta" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Magenta));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "Maroon" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Maroon));
    REQUIRE( Color.R == 0x80 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "MediumAquaMarine" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumAquaMarine));
    REQUIRE( Color.R == 0x66 );
    REQUIRE( Color.G == 0xCD );
    REQUIRE( Color.B == 0xAA );
  }
  SECTION( "MediumBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumBlue));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0xCD );
  }
  SECTION( "MediumOrchid" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumOrchid));
    REQUIRE( Color.R == 0xBA );
    REQUIRE( Color.G == 0x55 );
    REQUIRE( Color.B == 0xD3 );
  }
  SECTION( "MediumPurple" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumPurple));
    REQUIRE( Color.R == 0x93 );
    REQUIRE( Color.G == 0x70 );
    REQUIRE( Color.B == 0xDB );
  }
  SECTION( "MediumSeaGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumSeaGreen));
    REQUIRE( Color.R == 0x3C );
    REQUIRE( Color.G == 0xB3 );
    REQUIRE( Color.B == 0x71 );
  }
  SECTION( "MediumSlateBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumSlateBlue));
    REQUIRE( Color.R == 0x7B );
    REQUIRE( Color.G == 0x68 );
    REQUIRE( Color.B == 0xEE );
  }
  SECTION( "MediumSpringGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumSpringGreen));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xFA );
    REQUIRE( Color.B == 0x9A );
  }
  SECTION( "MediumTurquoise" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumTurquoise));
    REQUIRE( Color.R == 0x48 );
    REQUIRE( Color.G == 0xD1 );
    REQUIRE( Color.B == 0xCC );
  }
  SECTION( "MediumVioletRed" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MediumVioletRed));
    REQUIRE( Color.R == 0xC7 );
    REQUIRE( Color.G == 0x15 );
    REQUIRE( Color.B == 0x85 );
  }
  SECTION( "MidnightBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MidnightBlue));
    REQUIRE( Color.R == 0x19 );
    REQUIRE( Color.G == 0x19 );
    REQUIRE( Color.B == 0x70 );
  }
  SECTION( "MintCream" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MintCream));
    REQUIRE( Color.R == 0xF5 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFA );
  }
  SECTION( "MistyRose" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_MistyRose));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xE4 );
    REQUIRE( Color.B == 0xE1 );
  }
  SECTION( "Moccasin" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Moccasin));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xE4 );
    REQUIRE( Color.B == 0xB5 );
  }
  SECTION( "NavajoWhite" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_NavajoWhite));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xDE );
    REQUIRE( Color.B == 0xAD );
  }
  SECTION( "Navy" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Navy));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x80 );
  }
  SECTION( "OldLace" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_OldLace));
    REQUIRE( Color.R == 0xFD );
    REQUIRE( Color.G == 0xF5 );
    REQUIRE( Color.B == 0xE6 );
  }
  SECTION( "Olive" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Olive));
    REQUIRE( Color.R == 0x80 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "OliveDrab" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_OliveDrab));
    REQUIRE( Color.R == 0x6B );
    REQUIRE( Color.G == 0x8E );
    REQUIRE( Color.B == 0x23 );
  }
  SECTION( "Orange" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Orange));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xA5 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "OrangeRed" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_OrangeRed));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x45 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "Orchid" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Orchid));
    REQUIRE( Color.R == 0xDA );
    REQUIRE( Color.G == 0x70 );
    REQUIRE( Color.B == 0xD6 );
  }
  SECTION( "PaleGoldenRod" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PaleGoldenRod));
    REQUIRE( Color.R == 0xEE );
    REQUIRE( Color.G == 0xE8 );
    REQUIRE( Color.B == 0xAA );
  }
  SECTION( "PaleGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PaleGreen));
    REQUIRE( Color.R == 0x98 );
    REQUIRE( Color.G == 0xFB );
    REQUIRE( Color.B == 0x98 );
  }
  SECTION( "PaleTurquoise" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PaleTurquoise));
    REQUIRE( Color.R == 0xAF );
    REQUIRE( Color.G == 0xEE );
    REQUIRE( Color.B == 0xEE );
  }
  SECTION( "PaleVioletRed" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PaleVioletRed));
    REQUIRE( Color.R == 0xDB );
    REQUIRE( Color.G == 0x70 );
    REQUIRE( Color.B == 0x93 );
  }
  SECTION( "PapayaWhip" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PapayaWhip));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xEF );
    REQUIRE( Color.B == 0xD5 );
  }
  SECTION( "PeachPuff" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PeachPuff));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xDA );
    REQUIRE( Color.B == 0xB9 );
  }
  SECTION( "Peru" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Peru));
    REQUIRE( Color.R == 0xCD );
    REQUIRE( Color.G == 0x85 );
    REQUIRE( Color.B == 0x3F );
  }
  SECTION( "Pink" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Pink));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xC0 );
    REQUIRE( Color.B == 0xCB );
  }
  SECTION( "Plum" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Plum));
    REQUIRE( Color.R == 0xDD );
    REQUIRE( Color.G == 0xA0 );
    REQUIRE( Color.B == 0xDD );
  }
  SECTION( "PowderBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_PowderBlue));
    REQUIRE( Color.R == 0xB0 );
    REQUIRE( Color.G == 0xE0 );
    REQUIRE( Color.B == 0xE6 );
  }
  SECTION( "Purple" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Purple));
    REQUIRE( Color.R == 0x80 );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x80 );
  }
  SECTION( "RebeccaPurple" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_RebeccaPurple));
    REQUIRE( Color.R == 0x66 );
    REQUIRE( Color.G == 0x33 );
    REQUIRE( Color.B == 0x99 );
  }
  SECTION( "Red" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Red));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x00 );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "RosyBrown" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_RosyBrown));
    REQUIRE( Color.R == 0xBC );
    REQUIRE( Color.G == 0x8F );
    REQUIRE( Color.B == 0x8F );
  }
  SECTION( "RoyalBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_RoyalBlue));
    REQUIRE( Color.R == 0x41 );
    REQUIRE( Color.G == 0x69 );
    REQUIRE( Color.B == 0xE1 );
  }
  SECTION( "SaddleBrown" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SaddleBrown));
    REQUIRE( Color.R == 0x8B );
    REQUIRE( Color.G == 0x45 );
    REQUIRE( Color.B == 0x13 );
  }
  SECTION( "Salmon" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Salmon));
    REQUIRE( Color.R == 0xFA );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x72 );
  }
  SECTION( "SandyBrown" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SandyBrown));
    REQUIRE( Color.R == 0xF4 );
    REQUIRE( Color.G == 0xA4 );
    REQUIRE( Color.B == 0x60 );
  }
  SECTION( "SeaGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SeaGreen));
    REQUIRE( Color.R == 0x2E );
    REQUIRE( Color.G == 0x8B );
    REQUIRE( Color.B == 0x57 );
  }
  SECTION( "SeaShell" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SeaShell));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xF5 );
    REQUIRE( Color.B == 0xEE );
  }
  SECTION( "Sienna" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Sienna));
    REQUIRE( Color.R == 0xA0 );
    REQUIRE( Color.G == 0x52 );
    REQUIRE( Color.B == 0x2D );
  }
  SECTION( "Silver" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Silver));
    REQUIRE( Color.R == 0xC0 );
    REQUIRE( Color.G == 0xC0 );
    REQUIRE( Color.B == 0xC0 );
  }
  SECTION( "SkyBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SkyBlue));
    REQUIRE( Color.R == 0x87 );
    REQUIRE( Color.G == 0xCE );
    REQUIRE( Color.B == 0xEB );
  }
  SECTION( "SlateBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SlateBlue));
    REQUIRE( Color.R == 0x6A );
    REQUIRE( Color.G == 0x5A );
    REQUIRE( Color.B == 0xCD );
  }
  SECTION( "SlateGray" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SlateGray));
    REQUIRE( Color.R == 0x70 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x90 );
  }
  SECTION( "Snow" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Snow));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFA );
    REQUIRE( Color.B == 0xFA );
  }
  SECTION( "SpringGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SpringGreen));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0x7F );
  }
  SECTION( "SteelBlue" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_SteelBlue));
    REQUIRE( Color.R == 0x46 );
    REQUIRE( Color.G == 0x82 );
    REQUIRE( Color.B == 0xB4 );
  }
  SECTION( "Tan" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Tan));
    REQUIRE( Color.R == 0xD2 );
    REQUIRE( Color.G == 0xB4 );
    REQUIRE( Color.B == 0x8C );
  }
  SECTION( "Teal" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Teal));
    REQUIRE( Color.R == 0x00 );
    REQUIRE( Color.G == 0x80 );
    REQUIRE( Color.B == 0x80 );
  }
  SECTION( "Thistle" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Thistle));
    REQUIRE( Color.R == 0xD8 );
    REQUIRE( Color.G == 0xBF );
    REQUIRE( Color.B == 0xD8 );
  }
  SECTION( "Tomato" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Tomato));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0x63 );
    REQUIRE( Color.B == 0x47 );
  }
  SECTION( "Turquoise" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Turquoise));
    REQUIRE( Color.R == 0x40 );
    REQUIRE( Color.G == 0xE0 );
    REQUIRE( Color.B == 0xD0 );
  }
  SECTION( "Violet" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Violet));
    REQUIRE( Color.R == 0xEE );
    REQUIRE( Color.G == 0x82 );
    REQUIRE( Color.B == 0xEE );
  }
  SECTION( "Wheat" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Wheat));
    REQUIRE( Color.R == 0xF5 );
    REQUIRE( Color.G == 0xDE );
    REQUIRE( Color.B == 0xB3 );
  }
  SECTION( "White" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_White));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0xFF );
  }
  SECTION( "WhiteSmoke" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_WhiteSmoke));
    REQUIRE( Color.R == 0xF5 );
    REQUIRE( Color.G == 0xF5 );
    REQUIRE( Color.B == 0xF5 );
  }
  SECTION( "Yellow" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_Yellow));
    REQUIRE( Color.R == 0xFF );
    REQUIRE( Color.G == 0xFF );
    REQUIRE( Color.B == 0x00 );
  }
  SECTION( "YellowGreen" )
  {
    mtb_color Color = mtb_ToGammaColor(mtb_ToLinearColor(mtb_Color_YellowGreen));
    REQUIRE( Color.R == 0x9A );
    REQUIRE( Color.G == 0xCD );
    REQUIRE( Color.B == 0x32 );
  }
} // TEST_CASE "Pre-defined color values"
