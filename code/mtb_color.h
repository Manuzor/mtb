#if !defined(MTB_HEADER_mtb_color)
#define MTB_HEADER_mtb_color

#if defined(MTB_IMPLEMENTATION) && !defined(MTB_COLOR_IMPLEMENTATION)
  #define MTB_COLOR_IMPLEMENTATION
#endif

#if defined(MTB_COLOR_IMPLEMENTATION) && !defined(MTB_IMPLEMENTATION)
  #define MTB_IMPLEMENTATION
#endif

#include "mtb_common.h"


union mtb_linear_color
{
  struct
  {
    mtb_f32 R;
    mtb_f32 G;
    mtb_f32 B;
    mtb_f32 A;
  };

  mtb_f32 Data[4];
};

static_assert(sizeof(mtb_linear_color) == 16, "Incorrect size for mtb_linear_color.");

constexpr mtb_linear_color
mtb_LinearColor(mtb_f32 R, mtb_f32 G, mtb_f32 B, mtb_f32 A = 1.0f)
{
  return { R, G, B, A };
}

mtb_linear_color
mtb_LinearColorFromLinearHSV(mtb_f32 Hue, mtb_f32 Saturation, mtb_f32 Value);

mtb_linear_color
mtb_LinearColorFromGammaHSV(mtb_f32 Hue, mtb_f32 Saturation, mtb_f32 Value);

// http://en.wikipedia.org/wiki/Luminance_%28relative%29
constexpr mtb_f32
mtb_GetLuminance(mtb_linear_color Color)
{
  return 0.2126f * Color.R + 0.7152f * Color.G + 0.0722f * Color.B;
}

constexpr bool
mtb_IsColorNormalized(mtb_linear_color Color)
{
  return Color.R <= 1.0f && Color.G <= 1.0f && Color.B <= 1.0f && Color.A <= 1.0f &&
         Color.R >= 0.0f && Color.G >= 0.0f && Color.B >= 0.0f && Color.A >= 0.0f;
}

constexpr mtb_linear_color
mtb_GetInvertedColor(mtb_linear_color Color)
{
  return { 1.0f - Color.R, 1.0f - Color.G, 1.0f - Color.B, 1.0f - Color.A };
}

MTB_INLINE mtb_linear_color
mtb_GetSafeInvertedColor(mtb_linear_color Color)
{
  mtb_linear_color Result{};
  if(mtb_IsColorNormalized(Color))
  {
    Result = mtb_GetInvertedColor(Color);
  }
  return Result;
}

struct mtb_hsv
{
  mtb_f32 Hue;
  mtb_f32 Saturation;
  mtb_f32 Value;
};

mtb_hsv mtb_GetLinearHSV(mtb_linear_color Color);
mtb_hsv mtb_GetGammaHSV(mtb_linear_color Color);


union mtb_color
{
  struct
  {
    mtb_u08 R;
    mtb_u08 G;
    mtb_u08 B;
    mtb_u08 A;
  };

  mtb_u08 Data[4];
};

static_assert(sizeof(mtb_color) == 4, "Incorrect size for mtb_color.");

constexpr mtb_color
mtb_Color(mtb_u08 R, mtb_u08 G, mtb_u08 B, mtb_u08 A = 255)
{
  return { R, G, B, A };
}

MTB_INLINE mtb_f32
mtb_FromGammaToLinearColorSpace(mtb_f32 GammaValue)
{
  mtb_f32 LinearValue = GammaValue <= 0.04045f ? GammaValue / 12.92f : mtb_Pow((GammaValue + 0.055f) / 1.055f, 2.4f);
  return LinearValue;
}

MTB_INLINE mtb_f32
mtb_FromLinearToGammaColorSpace(mtb_f32 LinearValue)
{
  mtb_f32 GammaValue = LinearValue <= 0.0031308f ? 12.92f * LinearValue : 1.055f * mtb_Pow(LinearValue, 1.0f / 2.4f) - 0.055f;
  return GammaValue;
}

MTB_INLINE mtb_linear_color
mtb_ToLinearColor(mtb_color Color)
{
  mtb_linear_color Result {
    mtb_FromGammaToLinearColorSpace(mtb_Normalized_u08_To_f32(Color.R)),
    mtb_FromGammaToLinearColorSpace(mtb_Normalized_u08_To_f32(Color.G)),
    mtb_FromGammaToLinearColorSpace(mtb_Normalized_u08_To_f32(Color.B)),
    mtb_Normalized_u08_To_f32(Color.A)
  };
  return Result;
}

MTB_INLINE mtb_color
mtb_ToGammaColor(mtb_linear_color Color)
{
  mtb_color Result {
    mtb_Normalized_f32_To_u08(mtb_FromLinearToGammaColorSpace(Color.R)),
    mtb_Normalized_f32_To_u08(mtb_FromLinearToGammaColorSpace(Color.G)),
    mtb_Normalized_f32_To_u08(mtb_FromLinearToGammaColorSpace(Color.B)),
    mtb_Normalized_f32_To_u08(Color.A)
  };
  return Result;
}


//
// Pre-defined color values.
//

constexpr mtb_color mtb_Color_AliceBlue            = mtb_Color(0xF0, 0xF8, 0xFF);
constexpr mtb_color mtb_Color_AntiqueWhite         = mtb_Color(0xFA, 0xEB, 0xD7);
constexpr mtb_color mtb_Color_Aqua                 = mtb_Color(0x00, 0xFF, 0xFF);
constexpr mtb_color mtb_Color_Aquamarine           = mtb_Color(0x7F, 0xFF, 0xD4);
constexpr mtb_color mtb_Color_Azure                = mtb_Color(0xF0, 0xFF, 0xFF);
constexpr mtb_color mtb_Color_Beige                = mtb_Color(0xF5, 0xF5, 0xDC);
constexpr mtb_color mtb_Color_Bisque               = mtb_Color(0xFF, 0xE4, 0xC4);
constexpr mtb_color mtb_Color_Black                = mtb_Color(0x00, 0x00, 0x00);
constexpr mtb_color mtb_Color_BlanchedAlmond       = mtb_Color(0xFF, 0xEB, 0xCD);
constexpr mtb_color mtb_Color_Blue                 = mtb_Color(0x00, 0x00, 0xFF);
constexpr mtb_color mtb_Color_BlueViolet           = mtb_Color(0x8A, 0x2B, 0xE2);
constexpr mtb_color mtb_Color_Brown                = mtb_Color(0xA5, 0x2A, 0x2A);
constexpr mtb_color mtb_Color_BurlyWood            = mtb_Color(0xDE, 0xB8, 0x87);
constexpr mtb_color mtb_Color_CadetBlue            = mtb_Color(0x5F, 0x9E, 0xA0);
constexpr mtb_color mtb_Color_Chartreuse           = mtb_Color(0x7F, 0xFF, 0x00);
constexpr mtb_color mtb_Color_Chocolate            = mtb_Color(0xD2, 0x69, 0x1E);
constexpr mtb_color mtb_Color_Coral                = mtb_Color(0xFF, 0x7F, 0x50);
constexpr mtb_color mtb_Color_CornflowerBlue       = mtb_Color(0x64, 0x95, 0xED);
constexpr mtb_color mtb_Color_Cornsilk             = mtb_Color(0xFF, 0xF8, 0xDC);
constexpr mtb_color mtb_Color_Crimson              = mtb_Color(0xDC, 0x14, 0x3C);
constexpr mtb_color mtb_Color_Cyan                 = mtb_Color(0x00, 0xFF, 0xFF);
constexpr mtb_color mtb_Color_DarkBlue             = mtb_Color(0x00, 0x00, 0x8B);
constexpr mtb_color mtb_Color_DarkCyan             = mtb_Color(0x00, 0x8B, 0x8B);
constexpr mtb_color mtb_Color_DarkGoldenRod        = mtb_Color(0xB8, 0x86, 0x0B);
constexpr mtb_color mtb_Color_DarkGray             = mtb_Color(0xA9, 0xA9, 0xA9);
constexpr mtb_color mtb_Color_DarkGreen            = mtb_Color(0x00, 0x64, 0x00);
constexpr mtb_color mtb_Color_DarkKhaki            = mtb_Color(0xBD, 0xB7, 0x6B);
constexpr mtb_color mtb_Color_DarkMagenta          = mtb_Color(0x8B, 0x00, 0x8B);
constexpr mtb_color mtb_Color_DarkOliveGreen       = mtb_Color(0x55, 0x6B, 0x2F);
constexpr mtb_color mtb_Color_DarkOrange           = mtb_Color(0xFF, 0x8C, 0x00);
constexpr mtb_color mtb_Color_DarkOrchid           = mtb_Color(0x99, 0x32, 0xCC);
constexpr mtb_color mtb_Color_DarkRed              = mtb_Color(0x8B, 0x00, 0x00);
constexpr mtb_color mtb_Color_DarkSalmon           = mtb_Color(0xE9, 0x96, 0x7A);
constexpr mtb_color mtb_Color_DarkSeaGreen         = mtb_Color(0x8F, 0xBC, 0x8F);
constexpr mtb_color mtb_Color_DarkSlateBlue        = mtb_Color(0x48, 0x3D, 0x8B);
constexpr mtb_color mtb_Color_DarkSlateGray        = mtb_Color(0x2F, 0x4F, 0x4F);
constexpr mtb_color mtb_Color_DarkTurquoise        = mtb_Color(0x00, 0xCE, 0xD1);
constexpr mtb_color mtb_Color_DarkViolet           = mtb_Color(0x94, 0x00, 0xD3);
constexpr mtb_color mtb_Color_DeepPink             = mtb_Color(0xFF, 0x14, 0x93);
constexpr mtb_color mtb_Color_DeepSkyBlue          = mtb_Color(0x00, 0xBF, 0xFF);
constexpr mtb_color mtb_Color_DimGray              = mtb_Color(0x69, 0x69, 0x69);
constexpr mtb_color mtb_Color_DodgerBlue           = mtb_Color(0x1E, 0x90, 0xFF);
constexpr mtb_color mtb_Color_FireBrick            = mtb_Color(0xB2, 0x22, 0x22);
constexpr mtb_color mtb_Color_FloralWhite          = mtb_Color(0xFF, 0xFA, 0xF0);
constexpr mtb_color mtb_Color_ForestGreen          = mtb_Color(0x22, 0x8B, 0x22);
constexpr mtb_color mtb_Color_Fuchsia              = mtb_Color(0xFF, 0x00, 0xFF);
constexpr mtb_color mtb_Color_Gainsboro            = mtb_Color(0xDC, 0xDC, 0xDC);
constexpr mtb_color mtb_Color_GhostWhite           = mtb_Color(0xF8, 0xF8, 0xFF);
constexpr mtb_color mtb_Color_Gold                 = mtb_Color(0xFF, 0xD7, 0x00);
constexpr mtb_color mtb_Color_GoldenRod            = mtb_Color(0xDA, 0xA5, 0x20);
constexpr mtb_color mtb_Color_Gray                 = mtb_Color(0x80, 0x80, 0x80);
constexpr mtb_color mtb_Color_Green                = mtb_Color(0x00, 0x80, 0x00);
constexpr mtb_color mtb_Color_GreenYellow          = mtb_Color(0xAD, 0xFF, 0x2F);
constexpr mtb_color mtb_Color_HoneyDew             = mtb_Color(0xF0, 0xFF, 0xF0);
constexpr mtb_color mtb_Color_HotPink              = mtb_Color(0xFF, 0x69, 0xB4);
constexpr mtb_color mtb_Color_IndianRed            = mtb_Color(0xCD, 0x5C, 0x5C);
constexpr mtb_color mtb_Color_Indigo               = mtb_Color(0x4B, 0x00, 0x82);
constexpr mtb_color mtb_Color_Ivory                = mtb_Color(0xFF, 0xFF, 0xF0);
constexpr mtb_color mtb_Color_Khaki                = mtb_Color(0xF0, 0xE6, 0x8C);
constexpr mtb_color mtb_Color_Lavender             = mtb_Color(0xE6, 0xE6, 0xFA);
constexpr mtb_color mtb_Color_LavenderBlush        = mtb_Color(0xFF, 0xF0, 0xF5);
constexpr mtb_color mtb_Color_LawnGreen            = mtb_Color(0x7C, 0xFC, 0x00);
constexpr mtb_color mtb_Color_LemonChiffon         = mtb_Color(0xFF, 0xFA, 0xCD);
constexpr mtb_color mtb_Color_LightBlue            = mtb_Color(0xAD, 0xD8, 0xE6);
constexpr mtb_color mtb_Color_LightCoral           = mtb_Color(0xF0, 0x80, 0x80);
constexpr mtb_color mtb_Color_LightCyan            = mtb_Color(0xE0, 0xFF, 0xFF);
constexpr mtb_color mtb_Color_LightGoldenRodYellow = mtb_Color(0xFA, 0xFA, 0xD2);
constexpr mtb_color mtb_Color_LightGray            = mtb_Color(0xD3, 0xD3, 0xD3);
constexpr mtb_color mtb_Color_LightGreen           = mtb_Color(0x90, 0xEE, 0x90);
constexpr mtb_color mtb_Color_LightPink            = mtb_Color(0xFF, 0xB6, 0xC1);
constexpr mtb_color mtb_Color_LightSalmon          = mtb_Color(0xFF, 0xA0, 0x7A);
constexpr mtb_color mtb_Color_LightSeaGreen        = mtb_Color(0x20, 0xB2, 0xAA);
constexpr mtb_color mtb_Color_LightSkyBlue         = mtb_Color(0x87, 0xCE, 0xFA);
constexpr mtb_color mtb_Color_LightSlateGray       = mtb_Color(0x77, 0x88, 0x99);
constexpr mtb_color mtb_Color_LightSteelBlue       = mtb_Color(0xB0, 0xC4, 0xDE);
constexpr mtb_color mtb_Color_LightYellow          = mtb_Color(0xFF, 0xFF, 0xE0);
constexpr mtb_color mtb_Color_Lime                 = mtb_Color(0x00, 0xFF, 0x00);
constexpr mtb_color mtb_Color_LimeGreen            = mtb_Color(0x32, 0xCD, 0x32);
constexpr mtb_color mtb_Color_Linen                = mtb_Color(0xFA, 0xF0, 0xE6);
constexpr mtb_color mtb_Color_Magenta              = mtb_Color(0xFF, 0x00, 0xFF);
constexpr mtb_color mtb_Color_Maroon               = mtb_Color(0x80, 0x00, 0x00);
constexpr mtb_color mtb_Color_MediumAquaMarine     = mtb_Color(0x66, 0xCD, 0xAA);
constexpr mtb_color mtb_Color_MediumBlue           = mtb_Color(0x00, 0x00, 0xCD);
constexpr mtb_color mtb_Color_MediumOrchid         = mtb_Color(0xBA, 0x55, 0xD3);
constexpr mtb_color mtb_Color_MediumPurple         = mtb_Color(0x93, 0x70, 0xDB);
constexpr mtb_color mtb_Color_MediumSeaGreen       = mtb_Color(0x3C, 0xB3, 0x71);
constexpr mtb_color mtb_Color_MediumSlateBlue      = mtb_Color(0x7B, 0x68, 0xEE);
constexpr mtb_color mtb_Color_MediumSpringGreen    = mtb_Color(0x00, 0xFA, 0x9A);
constexpr mtb_color mtb_Color_MediumTurquoise      = mtb_Color(0x48, 0xD1, 0xCC);
constexpr mtb_color mtb_Color_MediumVioletRed      = mtb_Color(0xC7, 0x15, 0x85);
constexpr mtb_color mtb_Color_MidnightBlue         = mtb_Color(0x19, 0x19, 0x70);
constexpr mtb_color mtb_Color_MintCream            = mtb_Color(0xF5, 0xFF, 0xFA);
constexpr mtb_color mtb_Color_MistyRose            = mtb_Color(0xFF, 0xE4, 0xE1);
constexpr mtb_color mtb_Color_Moccasin             = mtb_Color(0xFF, 0xE4, 0xB5);
constexpr mtb_color mtb_Color_NavajoWhite          = mtb_Color(0xFF, 0xDE, 0xAD);
constexpr mtb_color mtb_Color_Navy                 = mtb_Color(0x00, 0x00, 0x80);
constexpr mtb_color mtb_Color_OldLace              = mtb_Color(0xFD, 0xF5, 0xE6);
constexpr mtb_color mtb_Color_Olive                = mtb_Color(0x80, 0x80, 0x00);
constexpr mtb_color mtb_Color_OliveDrab            = mtb_Color(0x6B, 0x8E, 0x23);
constexpr mtb_color mtb_Color_Orange               = mtb_Color(0xFF, 0xA5, 0x00);
constexpr mtb_color mtb_Color_OrangeRed            = mtb_Color(0xFF, 0x45, 0x00);
constexpr mtb_color mtb_Color_Orchid               = mtb_Color(0xDA, 0x70, 0xD6);
constexpr mtb_color mtb_Color_PaleGoldenRod        = mtb_Color(0xEE, 0xE8, 0xAA);
constexpr mtb_color mtb_Color_PaleGreen            = mtb_Color(0x98, 0xFB, 0x98);
constexpr mtb_color mtb_Color_PaleTurquoise        = mtb_Color(0xAF, 0xEE, 0xEE);
constexpr mtb_color mtb_Color_PaleVioletRed        = mtb_Color(0xDB, 0x70, 0x93);
constexpr mtb_color mtb_Color_PapayaWhip           = mtb_Color(0xFF, 0xEF, 0xD5);
constexpr mtb_color mtb_Color_PeachPuff            = mtb_Color(0xFF, 0xDA, 0xB9);
constexpr mtb_color mtb_Color_Peru                 = mtb_Color(0xCD, 0x85, 0x3F);
constexpr mtb_color mtb_Color_Pink                 = mtb_Color(0xFF, 0xC0, 0xCB);
constexpr mtb_color mtb_Color_Plum                 = mtb_Color(0xDD, 0xA0, 0xDD);
constexpr mtb_color mtb_Color_PowderBlue           = mtb_Color(0xB0, 0xE0, 0xE6);
constexpr mtb_color mtb_Color_Purple               = mtb_Color(0x80, 0x00, 0x80);
constexpr mtb_color mtb_Color_RebeccaPurple        = mtb_Color(0x66, 0x33, 0x99);
constexpr mtb_color mtb_Color_Red                  = mtb_Color(0xFF, 0x00, 0x00);
constexpr mtb_color mtb_Color_RosyBrown            = mtb_Color(0xBC, 0x8F, 0x8F);
constexpr mtb_color mtb_Color_RoyalBlue            = mtb_Color(0x41, 0x69, 0xE1);
constexpr mtb_color mtb_Color_SaddleBrown          = mtb_Color(0x8B, 0x45, 0x13);
constexpr mtb_color mtb_Color_Salmon               = mtb_Color(0xFA, 0x80, 0x72);
constexpr mtb_color mtb_Color_SandyBrown           = mtb_Color(0xF4, 0xA4, 0x60);
constexpr mtb_color mtb_Color_SeaGreen             = mtb_Color(0x2E, 0x8B, 0x57);
constexpr mtb_color mtb_Color_SeaShell             = mtb_Color(0xFF, 0xF5, 0xEE);
constexpr mtb_color mtb_Color_Sienna               = mtb_Color(0xA0, 0x52, 0x2D);
constexpr mtb_color mtb_Color_Silver               = mtb_Color(0xC0, 0xC0, 0xC0);
constexpr mtb_color mtb_Color_SkyBlue              = mtb_Color(0x87, 0xCE, 0xEB);
constexpr mtb_color mtb_Color_SlateBlue            = mtb_Color(0x6A, 0x5A, 0xCD);
constexpr mtb_color mtb_Color_SlateGray            = mtb_Color(0x70, 0x80, 0x90);
constexpr mtb_color mtb_Color_Snow                 = mtb_Color(0xFF, 0xFA, 0xFA);
constexpr mtb_color mtb_Color_SpringGreen          = mtb_Color(0x00, 0xFF, 0x7F);
constexpr mtb_color mtb_Color_SteelBlue            = mtb_Color(0x46, 0x82, 0xB4);
constexpr mtb_color mtb_Color_Tan                  = mtb_Color(0xD2, 0xB4, 0x8C);
constexpr mtb_color mtb_Color_Teal                 = mtb_Color(0x00, 0x80, 0x80);
constexpr mtb_color mtb_Color_Thistle              = mtb_Color(0xD8, 0xBF, 0xD8);
constexpr mtb_color mtb_Color_Tomato               = mtb_Color(0xFF, 0x63, 0x47);
constexpr mtb_color mtb_Color_Turquoise            = mtb_Color(0x40, 0xE0, 0xD0);
constexpr mtb_color mtb_Color_Violet               = mtb_Color(0xEE, 0x82, 0xEE);
constexpr mtb_color mtb_Color_Wheat                = mtb_Color(0xF5, 0xDE, 0xB3);
constexpr mtb_color mtb_Color_White                = mtb_Color(0xFF, 0xFF, 0xFF);
constexpr mtb_color mtb_Color_WhiteSmoke           = mtb_Color(0xF5, 0xF5, 0xF5);
constexpr mtb_color mtb_Color_Yellow               = mtb_Color(0xFF, 0xFF, 0x00);
constexpr mtb_color mtb_Color_YellowGreen          = mtb_Color(0x9A, 0xCD, 0x32);

#endif // !defined(MTB_HEADER_mtb_color)


// ==============
// Implementation
// ==============

#if defined(MTB_COLOR_IMPLEMENTATION)
#if !defined(MTB_IMPL_mtb_color)
#define MTB_IMPL_mtb_color

mtb_linear_color
mtb_LinearColorFromLinearHSV(mtb_f32 Hue, mtb_f32 Saturation, mtb_f32 Value)
{
  // http://www.rapidtables.com/convert/color/hsv-to-rgb.htm

  mtb_linear_color Result{};

  if((Hue >= 0 || Hue <= 360) &&(Saturation >= 0 || Saturation <= 1) && (Value >= 0 || Value <= 1))
  {
    mtb_f32 const C = Saturation * Value;
    mtb_f32 const X = C * (1.0f - mtb_Abs(mtb_Mod(Hue / 60.0f, 2) - 1.0f));
    mtb_f32 const M = Value - C;

    Result.A = 1.0f;

    if (Hue < 60)
    {
      Result.R = C + M;
      Result.G = X + M;
      Result.B = 0 + M;
    }
    else if (Hue < 120)
    {
      Result.R = X + M;
      Result.G = C + M;
      Result.B = 0 + M;
    }
    else if (Hue < 180)
    {
      Result.R = 0 + M;
      Result.G = C + M;
      Result.B = X + M;
    }
    else if (Hue < 240)
    {
      Result.R = 0 + M;
      Result.G = X + M;
      Result.B = C + M;
    }
    else if (Hue < 300)
    {
      Result.R = X + M;
      Result.G = 0 + M;
      Result.B = C + M;
    }
    else
    {
      Result.R = C + M;
      Result.G = 0 + M;
      Result.B = X + M;
    }
  }
  else
  {
    // TODO: Values are out of range. Diagnostics?
  }


  return Result;
}

mtb_linear_color
mtb_LinearColorFromGammaHSV(mtb_f32 Hue, mtb_f32 Saturation, mtb_f32 Value)
{
  mtb_linear_color LinearColorWithGammaValues = mtb_LinearColorFromLinearHSV(Hue, Saturation, Value);

  mtb_linear_color Result;
  Result.R = mtb_FromGammaToLinearColorSpace(LinearColorWithGammaValues.R);
  Result.G = mtb_FromGammaToLinearColorSpace(LinearColorWithGammaValues.G);
  Result.B = mtb_FromGammaToLinearColorSpace(LinearColorWithGammaValues.B);
  Result.A = LinearColorWithGammaValues.A;

  return Result;
}

mtb_hsv
mtb_GetLinearHSV(mtb_linear_color Color)
{
  // http://en.literateprograms.org/RGB_to_HSV_color_space_conversion_%28C%29

  mtb_hsv Result{};
  Result.Value = mtb_Max(Color.R, mtb_Max(Color.G, Color.B));

  if(Result.Value != 0)
  {
    mtb_f32 const InvValue = 1.0f / Result.Value;
    mtb_f32 Norm_R = Color.R * InvValue;
    mtb_f32 Norm_G = Color.G * InvValue;
    mtb_f32 Norm_B = Color.B * InvValue;
    mtb_f32 const RGB_Min = mtb_Min(Norm_R, mtb_Min(Norm_G, Norm_B));
    mtb_f32 const RGB_Max = mtb_Max(Norm_R, mtb_Max(Norm_G, Norm_B));

    Result.Saturation = RGB_Max - RGB_Min;

    if(Result.Saturation != 0)
    {
      // Normalize saturation
      mtb_f32 const RGB_Delta_Inverse = 1.0f / Result.Saturation;
      Norm_R = (Norm_R - RGB_Min) * RGB_Delta_Inverse;
      Norm_G = (Norm_G - RGB_Min) * RGB_Delta_Inverse;
      Norm_B = (Norm_B - RGB_Min) * RGB_Delta_Inverse;

      // Hue
      if(RGB_Max == Norm_R)
      {
        Result.Hue = 60.0f * (Norm_G - Norm_B);

        if(Result.Hue < 0)
          Result.Hue += 360.0f;
      }
      else if(RGB_Max == Norm_G)
      {
        Result.Hue = 120.0f + 60.0f * (Norm_B - Norm_R);
      }
      else
      {
        Result.Hue = 240.0f + 60.0f * (Norm_R - Norm_G);
      }
    }
  }

  return Result;
}

mtb_hsv
mtb_GetGammaHSV(mtb_linear_color Color)
{
  mtb_f32 GammaR = mtb_FromLinearToGammaColorSpace(Color.R);
  mtb_f32 GammaG = mtb_FromLinearToGammaColorSpace(Color.G);
  mtb_f32 GammaB = mtb_FromLinearToGammaColorSpace(Color.B);

  mtb_linear_color LinearColorWithGammaValues = mtb_LinearColor(GammaR, GammaG, GammaB, Color.A);
  mtb_hsv Result = mtb_GetLinearHSV(LinearColorWithGammaValues);
  return Result;
}


#endif // !defined(MTB_IMPL_mtb_color)
#endif // defined(MTB_COLOR_IMPLEMENTATION)
