// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <gcem.hpp>
#include <wpi/StringExtras.h>
#include <wpi/ct_string.h>

namespace frc {

/**
 * Represents colors that can be used with Addressable LEDs.
 *
 * Limited to 12 bits of precision.
 */
class Color {
 public:
  /*
   * FIRST Colors
   */

  /**
   * 0x1560BD.
   */
  static const Color DENIM;

  /**
   * 0x0066B3.
   */
  static const Color FIRST_BLUE;

  /**
   * 0xED1C24.
   */
  static const Color FIRST_RED;

  /*
   * Standard Colors
   */

  /**
   * 0xF0F8FF.
   */
  static const Color ALICE_BLUE;

  /**
   * 0xFAEBD7.
   */
  static const Color ANTIQUE_WHITE;

  /**
   * 0x00FFFF.
   */
  static const Color AQUA;

  /**
   * 0x7FFFD4.
   */
  static const Color AQUAMARINE;

  /**
   * 0xF0FFFF.
   */
  static const Color AZURE;

  /**
   * 0xF5F5DC.
   */
  static const Color BEIGE;

  /**
   * 0xFFE4C4.
   */
  static const Color BISQUE;

  /**
   * 0x000000.
   */
  static const Color BLACK;

  /**
   * 0xFFEBCD.
   */
  static const Color BLANCHED_ALMOND;

  /**
   * 0x0000FF.
   */
  static const Color BLUE;

  /**
   * 0x8A2BE2.
   */
  static const Color BLUE_VIOLET;

  /**
   * 0xA52A2A.
   */
  static const Color BROWN;

  /**
   * 0xDEB887.
   */
  static const Color BURLYWOOD;

  /**
   * 0x5F9EA0.
   */
  static const Color CADET_BLUE;

  /**
   * 0x7FFF00.
   */
  static const Color CHARTREUSE;

  /**
   * 0xD2691E.
   */
  static const Color CHOCOLATE;

  /**
   * 0xFF7F50.
   */
  static const Color CORAL;

  /**
   * 0x6495ED.
   */
  static const Color CORNFLOWER_BLUE;

  /**
   * 0xFFF8DC.
   */
  static const Color CORNSILK;

  /**
   * 0xDC143C.
   */
  static const Color CRIMSON;

  /**
   * 0x00FFFF.
   */
  static const Color CYAN;

  /**
   * 0x00008B.
   */
  static const Color DARK_BLUE;

  /**
   * 0x008B8B.
   */
  static const Color DARK_CYAN;

  /**
   * 0xB8860B.
   */
  static const Color DARK_GOLDENROD;

  /**
   * 0xA9A9A9.
   */
  static const Color DARK_GRAY;

  /**
   * 0x006400.
   */
  static const Color DARK_GREEN;

  /**
   * 0xBDB76B.
   */
  static const Color DARK_KHAKI;

  /**
   * 0x8B008B.
   */
  static const Color DARK_MAGENTA;

  /**
   * 0x556B2F.
   */
  static const Color DARK_OLIVE_GREEN;

  /**
   * 0xFF8C00.
   */
  static const Color DARK_ORANGE;

  /**
   * 0x9932CC.
   */
  static const Color DARK_ORCHID;

  /**
   * 0x8B0000.
   */
  static const Color DARK_RED;

  /**
   * 0xE9967A.
   */
  static const Color DARK_SALMON;

  /**
   * 0x8FBC8F.
   */
  static const Color DARK_SEA_GREEN;

  /**
   * 0x483D8B.
   */
  static const Color DARK_SLATE_BLUE;

  /**
   * 0x2F4F4F.
   */
  static const Color DARK_SLATE_GRAY;

  /**
   * 0x00CED1.
   */
  static const Color DARK_TURQUOISE;

  /**
   * 0x9400D3.
   */
  static const Color DARK_VIOLET;

  /**
   * 0xFF1493.
   */
  static const Color DEEP_PINK;

  /**
   * 0x00BFFF.
   */
  static const Color DEEP_SKY_BLUE;

  /**
   * 0x696969.
   */
  static const Color DIM_GRAY;

  /**
   * 0x1E90FF.
   */
  static const Color DODGER_BLUE;

  /**
   * 0xB22222.
   */
  static const Color FIREBRICK;

  /**
   * 0xFFFAF0.
   */
  static const Color FLORAL_WHITE;

  /**
   * 0x228B22.
   */
  static const Color FOREST_GREEN;

  /**
   * 0xFF00FF.
   */
  static const Color FUCHSIA;

  /**
   * 0xDCDCDC.
   */
  static const Color GAINSBORO;

  /**
   * 0xF8F8FF.
   */
  static const Color GHOST_WHITE;

  /**
   * 0xFFD700.
   */
  static const Color GOLD;

  /**
   * 0xDAA520.
   */
  static const Color GOLDENROD;

  /**
   * 0x808080.
   */
  static const Color GRAY;

  /**
   * 0x008000.
   */
  static const Color GREEN;

  /**
   * 0xADFF2F.
   */
  static const Color GREEN_YELLOW;

  /**
   * 0xF0FFF0.
   */
  static const Color HONEYDEW;

  /**
   * 0xFF69B4.
   */
  static const Color HOT_PINK;

  /**
   * 0xCD5C5C.
   */
  static const Color INDIAN_RED;

  /**
   * 0x4B0082.
   */
  static const Color INDIGO;

  /**
   * 0xFFFFF0.
   */
  static const Color IVORY;

  /**
   * 0xF0E68C.
   */
  static const Color KHAKI;

  /**
   * 0xE6E6FA.
   */
  static const Color LAVENDER;

  /**
   * 0xFFF0F5.
   */
  static const Color LAVENDER_BLUSH;

  /**
   * 0x7CFC00.
   */
  static const Color LAWN_GREEN;

  /**
   * 0xFFFACD.
   */
  static const Color LEMON_CHIFFON;

  /**
   * 0xADD8E6.
   */
  static const Color LIGHT_BLUE;

  /**
   * 0xF08080.
   */
  static const Color LIGHT_CORAL;

  /**
   * 0xE0FFFF.
   */
  static const Color LIGHT_CYAN;

  /**
   * 0xFAFAD2.
   */
  static const Color LIGHT_GOLDENROD_YELLOW;

  /**
   * 0xD3D3D3.
   */
  static const Color LIGHT_GRAY;

  /**
   * 0x90EE90.
   */
  static const Color LIGHT_GREEN;

  /**
   * 0xFFB6C1.
   */
  static const Color LIGHT_PINK;

  /**
   * 0xFFA07A.
   */
  static const Color LIGHT_SALMON;

  /**
   * 0x20B2AA.
   */
  static const Color LIGHT_SEA_GREEN;

  /**
   * 0x87CEFA.
   */
  static const Color LIGHT_SKY_BLUE;

  /**
   * 0x778899.
   */
  static const Color LIGHT_SLATE_GRAY;

  /**
   * 0xB0C4DE.
   */
  static const Color LIGHT_STEEL_BLUE;

  /**
   * 0xFFFFE0.
   */
  static const Color LIGHT_YELLOW;

  /**
   * 0x00FF00.
   */
  static const Color LIME;

  /**
   * 0x32CD32.
   */
  static const Color LIME_GREEN;

  /**
   * 0xFAF0E6.
   */
  static const Color LINEN;

  /**
   * 0xFF00FF.
   */
  static const Color MAGENTA;

  /**
   * 0x800000.
   */
  static const Color MAROON;

  /**
   * 0x66CDAA.
   */
  static const Color MEDIUM_AQUAMARINE;

  /**
   * 0x0000CD.
   */
  static const Color MEDIUM_BLUE;

  /**
   * 0xBA55D3.
   */
  static const Color MEDIUM_ORCHID;

  /**
   * 0x9370DB.
   */
  static const Color MEDIUM_PURPLE;

  /**
   * 0x3CB371.
   */
  static const Color MEDIUM_SEA_GREEN;

  /**
   * 0x7B68EE.
   */
  static const Color MEDIUM_SLATE_BLUE;

  /**
   * 0x00FA9A.
   */
  static const Color MEDIUM_SPRING_GREEN;

  /**
   * 0x48D1CC.
   */
  static const Color MEDIUM_TURQUOISE;

  /**
   * 0xC71585.
   */
  static const Color MEDIUM_VIOLET_RED;

  /**
   * 0x191970.
   */
  static const Color MIDNIGHT_BLUE;

  /**
   * 0xF5FFFA.
   */
  static const Color MINTCREAM;

  /**
   * 0xFFE4E1.
   */
  static const Color MISTY_ROSE;

  /**
   * 0xFFE4B5.
   */
  static const Color MOCCASIN;

  /**
   * 0xFFDEAD.
   */
  static const Color NAVAJO_WHITE;

  /**
   * 0x000080.
   */
  static const Color NAVY;

  /**
   * 0xFDF5E6.
   */
  static const Color OLD_LACE;

  /**
   * 0x808000.
   */
  static const Color OLIVE;

  /**
   * 0x6B8E23.
   */
  static const Color OLIVE_DRAB;

  /**
   * 0xFFA500.
   */
  static const Color ORANGE;

  /**
   * 0xFF4500.
   */
  static const Color ORANGE_RED;

  /**
   * 0xDA70D6.
   */
  static const Color ORCHID;

  /**
   * 0xEEE8AA.
   */
  static const Color PALE_GOLDENROD;

  /**
   * 0x98FB98.
   */
  static const Color PALE_GREEN;

  /**
   * 0xAFEEEE.
   */
  static const Color PALE_TURQUOISE;

  /**
   * 0xDB7093.
   */
  static const Color PALE_VIOLET_RED;

  /**
   * 0xFFEFD5.
   */
  static const Color PAPAYA_WHIP;

  /**
   * 0xFFDAB9.
   */
  static const Color PEACH_PUFF;

  /**
   * 0xCD853F.
   */
  static const Color PERU;

  /**
   * 0xFFC0CB.
   */
  static const Color PINK;

  /**
   * 0xDDA0DD.
   */
  static const Color PLUM;

  /**
   * 0xB0E0E6.
   */
  static const Color POWDER_BLUE;

  /**
   * 0x800080.
   */
  static const Color PURPLE;

  /**
   * 0xFF0000.
   */
  static const Color RED;

  /**
   * 0xBC8F8F.
   */
  static const Color ROSY_BROWN;

  /**
   * 0x4169E1.
   */
  static const Color ROYAL_BLUE;

  /**
   * 0x8B4513.
   */
  static const Color SADDLE_BROWN;

  /**
   * 0xFA8072.
   */
  static const Color SALMON;

  /**
   * 0xF4A460.
   */
  static const Color SANDY_BROWN;

  /**
   * 0x2E8B57.
   */
  static const Color SEA_GREEN;

  /**
   * 0xFFF5EE.
   */
  static const Color SEASHELL;

  /**
   * 0xA0522D.
   */
  static const Color SIENNA;

  /**
   * 0xC0C0C0.
   */
  static const Color SILVER;

  /**
   * 0x87CEEB.
   */
  static const Color SKY_BLUE;

  /**
   * 0x6A5ACD.
   */
  static const Color SLATE_BLUE;

  /**
   * 0x708090.
   */
  static const Color SLATE_GRAY;

  /**
   * 0xFFFAFA.
   */
  static const Color SNOW;

  /**
   * 0x00FF7F.
   */
  static const Color SPRING_GREEN;

  /**
   * 0x4682B4.
   */
  static const Color STEEL_BLUE;

  /**
   * 0xD2B48C.
   */
  static const Color TAN;

  /**
   * 0x008080.
   */
  static const Color TEAL;

  /**
   * 0xD8BFD8.
   */
  static const Color THISTLE;

  /**
   * 0xFF6347.
   */
  static const Color TOMATO;

  /**
   * 0x40E0D0.
   */
  static const Color TURQUOISE;

  /**
   * 0xEE82EE.
   */
  static const Color VIOLET;

  /**
   * 0xF5DEB3.
   */
  static const Color WHEAT;

  /**
   * 0xFFFFFF.
   */
  static const Color WHITE;

  /**
   * 0xF5F5F5.
   */
  static const Color WHITE_SMOKE;

  /**
   * 0xFFFF00.
   */
  static const Color YELLOW;

  /**
   * 0x9ACD32.
   */
  static const Color YELLOW_GREEN;

  /**
   * Constructs a default color (black).
   */
  constexpr Color() = default;

  /**
   * Constructs a Color from doubles (0-1).
   *
   * @param r Red value (0-1)
   * @param g Green value (0-1)
   * @param b Blue value (0-1)
   */
  constexpr Color(double r, double g, double b)
      : red(roundAndClamp(r)),
        green(roundAndClamp(g)),
        blue(roundAndClamp(b)) {}

  /**
   * Constructs a Color from ints (0-255).
   *
   * @param r Red value (0-255)
   * @param g Green value (0-255)
   * @param b Blue value (0-255)
   */
  constexpr Color(int r, int g, int b)
      : Color(r / 255.0, g / 255.0, b / 255.0) {}

  /**
   * Constructs a Color from a hex string.
   *
   * @param hexString a string of the format <tt>\#RRGGBB</tt>
   * @throws std::invalid_argument if the hex string is invalid.
   */
  explicit constexpr Color(std::string_view hexString) {
    if (hexString.length() != 7 || !hexString.starts_with("#") ||
        !wpi::isHexDigit(hexString[1]) || !wpi::isHexDigit(hexString[2]) ||
        !wpi::isHexDigit(hexString[3]) || !wpi::isHexDigit(hexString[4]) ||
        !wpi::isHexDigit(hexString[5]) || !wpi::isHexDigit(hexString[6])) {
      throw std::invalid_argument(
          fmt::format("Invalid hex string for Color \"{}\"", hexString));
    }

    int r = wpi::hexDigitValue(hexString[1]) * 16 +
            wpi::hexDigitValue(hexString[2]);
    int g = wpi::hexDigitValue(hexString[3]) * 16 +
            wpi::hexDigitValue(hexString[4]);
    int b = wpi::hexDigitValue(hexString[5]) * 16 +
            wpi::hexDigitValue(hexString[6]);

    red = r / 255.0;
    green = g / 255.0;
    blue = b / 255.0;
  }

  constexpr bool operator==(const Color&) const = default;

  /**
   * Creates a Color from HSV values.
   *
   * @param h The h value [0-180)
   * @param s The s value [0-255]
   * @param v The v value [0-255]
   * @return The color
   */
  static constexpr Color FromHSV(int h, int s, int v) {
    // Loosely based on
    // https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
    // The hue range is split into 60 degree regions where in each region there
    // is one rgb component at a low value (m), one at a high value (v) and one
    // that changes (X) from low to high (X+m) or high to low (v-X)

    // Difference between highest and lowest value of any rgb component
    int chroma = (s * v) >> 8;

    // Because hue is 0-180 rather than 0-360 use 30 not 60
    int region = (h / 30) % 6;

    // Remainder converted from 0-30 to 0-255
    int remainder = static_cast<int>((h % 30) * (255 / 30.0));

    // Value of the lowest rgb component
    int m = v - chroma;

    // Goes from 0 to chroma as hue increases
    int X = (chroma * remainder) >> 8;

    switch (region) {
      case 0:
        return Color(v, X + m, m);
      case 1:
        return Color(v - X, v, m);
      case 2:
        return Color(m, v, X + m);
      case 3:
        return Color(m, v - X, v);
      case 4:
        return Color(X + m, m, v);
      default:
        return Color(v, m, v - X);
    }
  }

  /**
   * Return this color represented as a hex string.
   *
   * @return a string of the format <tt>\#RRGGBB</tt>
   */
  constexpr auto HexString() const {
    const int r = 255.0 * red;
    const int g = 255.0 * green;
    const int b = 255.0 * blue;

    return wpi::ct_string<char, std::char_traits<char>, 7>{
        {'#', wpi::hexdigit(r / 16), wpi::hexdigit(r % 16),
         wpi::hexdigit(g / 16), wpi::hexdigit(g % 16), wpi::hexdigit(b / 16),
         wpi::hexdigit(b % 16)}};
  }

  /// Red component (0-1).
  double red = 0.0;

  /// Green component (0-1).
  double green = 0.0;

  /// Blue component (0-1).
  double blue = 0.0;

 private:
  static constexpr double roundAndClamp(double value) {
    return std::clamp(gcem::ceil(value * (1 << 12)) / (1 << 12), 0.0, 1.0);
  }
};

/*
 * FIRST Colors
 */
inline constexpr Color Color::DENIM{0.0823529412, 0.376470589, 0.7411764706};
inline constexpr Color Color::FIRST_BLUE{0.0, 0.4, 0.7019607844};
inline constexpr Color Color::FIRST_RED{0.9294117648, 0.1098039216,
                                        0.1411764706};

/*
 * Standard Colors
 */
inline constexpr Color Color::ALICE_BLUE{0.9411765f, 0.972549f, 1.0f};
inline constexpr Color Color::ANTIQUE_WHITE{0.98039216f, 0.92156863f,
                                            0.84313726f};
inline constexpr Color Color::AQUA{0.0f, 1.0f, 1.0f};
inline constexpr Color Color::AQUAMARINE{0.49803922f, 1.0f, 0.83137256f};
inline constexpr Color Color::AZURE{0.9411765f, 1.0f, 1.0f};
inline constexpr Color Color::BEIGE{0.9607843f, 0.9607843f, 0.8627451f};
inline constexpr Color Color::BISQUE{1.0f, 0.89411765f, 0.76862746f};
inline constexpr Color Color::BLACK{0.0f, 0.0f, 0.0f};
inline constexpr Color Color::BLANCHED_ALMOND{1.0f, 0.92156863f, 0.8039216f};
inline constexpr Color Color::BLUE{0.0f, 0.0f, 1.0f};
inline constexpr Color Color::BLUE_VIOLET{0.5411765f, 0.16862746f, 0.8862745f};
inline constexpr Color Color::BROWN{0.64705884f, 0.16470589f, 0.16470589f};
inline constexpr Color Color::BURLYWOOD{0.87058824f, 0.72156864f, 0.5294118f};
inline constexpr Color Color::CADET_BLUE{0.37254903f, 0.61960787f, 0.627451f};
inline constexpr Color Color::CHARTREUSE{0.49803922f, 1.0f, 0.0f};
inline constexpr Color Color::CHOCOLATE{0.8235294f, 0.4117647f, 0.11764706f};
inline constexpr Color Color::CORAL{1.0f, 0.49803922f, 0.3137255f};
inline constexpr Color Color::CORNFLOWER_BLUE{0.39215687f, 0.58431375f,
                                              0.92941177f};
inline constexpr Color Color::CORNSILK{1.0f, 0.972549f, 0.8627451f};
inline constexpr Color Color::CRIMSON{0.8627451f, 0.078431375f, 0.23529412f};
inline constexpr Color Color::CYAN{0.0f, 1.0f, 1.0f};
inline constexpr Color Color::DARK_BLUE{0.0f, 0.0f, 0.54509807f};
inline constexpr Color Color::DARK_CYAN{0.0f, 0.54509807f, 0.54509807f};
inline constexpr Color Color::DARK_GOLDENROD{0.72156864f, 0.5254902f,
                                             0.043137256f};
inline constexpr Color Color::DARK_GRAY{0.6627451f, 0.6627451f, 0.6627451f};
inline constexpr Color Color::DARK_GREEN{0.0f, 0.39215687f, 0.0f};
inline constexpr Color Color::DARK_KHAKI{0.7411765f, 0.7176471f, 0.41960785f};
inline constexpr Color Color::DARK_MAGENTA{0.54509807f, 0.0f, 0.54509807f};
inline constexpr Color Color::DARK_OLIVE_GREEN{0.33333334f, 0.41960785f,
                                               0.18431373f};
inline constexpr Color Color::DARK_ORANGE{1.0f, 0.54901963f, 0.0f};
inline constexpr Color Color::DARK_ORCHID{0.6f, 0.19607843f, 0.8f};
inline constexpr Color Color::DARK_RED{0.54509807f, 0.0f, 0.0f};
inline constexpr Color Color::DARK_SALMON{0.9137255f, 0.5882353f, 0.47843137f};
inline constexpr Color Color::DARK_SEA_GREEN{0.56078434f, 0.7372549f,
                                             0.56078434f};
inline constexpr Color Color::DARK_SLATE_BLUE{0.28235295f, 0.23921569f,
                                              0.54509807f};
inline constexpr Color Color::DARK_SLATE_GRAY{0.18431373f, 0.30980393f,
                                              0.30980393f};
inline constexpr Color Color::DARK_TURQUOISE{0.0f, 0.80784315f, 0.81960785f};
inline constexpr Color Color::DARK_VIOLET{0.5803922f, 0.0f, 0.827451f};
inline constexpr Color Color::DEEP_PINK{1.0f, 0.078431375f, 0.5764706f};
inline constexpr Color Color::DEEP_SKY_BLUE{0.0f, 0.7490196f, 1.0f};
inline constexpr Color Color::DIM_GRAY{0.4117647f, 0.4117647f, 0.4117647f};
inline constexpr Color Color::DODGER_BLUE{0.11764706f, 0.5647059f, 1.0f};
inline constexpr Color Color::FIREBRICK{0.69803923f, 0.13333334f, 0.13333334f};
inline constexpr Color Color::FLORAL_WHITE{1.0f, 0.98039216f, 0.9411765f};
inline constexpr Color Color::FOREST_GREEN{0.13333334f, 0.54509807f,
                                           0.13333334f};
inline constexpr Color Color::FUCHSIA{1.0f, 0.0f, 1.0f};
inline constexpr Color Color::GAINSBORO{0.8627451f, 0.8627451f, 0.8627451f};
inline constexpr Color Color::GHOST_WHITE{0.972549f, 0.972549f, 1.0f};
inline constexpr Color Color::GOLD{1.0f, 0.84313726f, 0.0f};
inline constexpr Color Color::GOLDENROD{0.85490197f, 0.64705884f, 0.1254902f};
inline constexpr Color Color::GRAY{0.5019608f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::GREEN{0.0f, 0.5019608f, 0.0f};
inline constexpr Color Color::GREEN_YELLOW{0.6784314f, 1.0f, 0.18431373f};
inline constexpr Color Color::HONEYDEW{0.9411765f, 1.0f, 0.9411765f};
inline constexpr Color Color::HOT_PINK{1.0f, 0.4117647f, 0.7058824f};
inline constexpr Color Color::INDIAN_RED{0.8039216f, 0.36078432f, 0.36078432f};
inline constexpr Color Color::INDIGO{0.29411766f, 0.0f, 0.50980395f};
inline constexpr Color Color::IVORY{1.0f, 1.0f, 0.9411765f};
inline constexpr Color Color::KHAKI{0.9411765f, 0.9019608f, 0.54901963f};
inline constexpr Color Color::LAVENDER{0.9019608f, 0.9019608f, 0.98039216f};
inline constexpr Color Color::LAVENDER_BLUSH{1.0f, 0.9411765f, 0.9607843f};
inline constexpr Color Color::LAWN_GREEN{0.4862745f, 0.9882353f, 0.0f};
inline constexpr Color Color::LEMON_CHIFFON{1.0f, 0.98039216f, 0.8039216f};
inline constexpr Color Color::LIGHT_BLUE{0.6784314f, 0.84705883f, 0.9019608f};
inline constexpr Color Color::LIGHT_CORAL{0.9411765f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::LIGHT_CYAN{0.8784314f, 1.0f, 1.0f};
inline constexpr Color Color::LIGHT_GOLDENROD_YELLOW{0.98039216f, 0.98039216f,
                                                     0.8235294f};
inline constexpr Color Color::LIGHT_GRAY{0.827451f, 0.827451f, 0.827451f};
inline constexpr Color Color::LIGHT_GREEN{0.5647059f, 0.93333334f, 0.5647059f};
inline constexpr Color Color::LIGHT_PINK{1.0f, 0.7137255f, 0.75686276f};
inline constexpr Color Color::LIGHT_SALMON{1.0f, 0.627451f, 0.47843137f};
inline constexpr Color Color::LIGHT_SEA_GREEN{0.1254902f, 0.69803923f,
                                              0.6666667f};
inline constexpr Color Color::LIGHT_SKY_BLUE{0.5294118f, 0.80784315f,
                                             0.98039216f};
inline constexpr Color Color::LIGHT_SLATE_GRAY{0.46666667f, 0.53333336f, 0.6f};
inline constexpr Color Color::LIGHT_STEEL_BLUE{0.6901961f, 0.76862746f,
                                               0.87058824f};
inline constexpr Color Color::LIGHT_YELLOW{1.0f, 1.0f, 0.8784314f};
inline constexpr Color Color::LIME{0.0f, 1.0f, 0.0f};
inline constexpr Color Color::LIME_GREEN{0.19607843f, 0.8039216f, 0.19607843f};
inline constexpr Color Color::LINEN{0.98039216f, 0.9411765f, 0.9019608f};
inline constexpr Color Color::MAGENTA{1.0f, 0.0f, 1.0f};
inline constexpr Color Color::MAROON{0.5019608f, 0.0f, 0.0f};
inline constexpr Color Color::MEDIUM_AQUAMARINE{0.4f, 0.8039216f, 0.6666667f};
inline constexpr Color Color::MEDIUM_BLUE{0.0f, 0.0f, 0.8039216f};
inline constexpr Color Color::MEDIUM_ORCHID{0.7294118f, 0.33333334f, 0.827451f};
inline constexpr Color Color::MEDIUM_PURPLE{0.5764706f, 0.4392157f,
                                            0.85882354f};
inline constexpr Color Color::MEDIUM_SEA_GREEN{0.23529412f, 0.7019608f,
                                               0.44313726f};
inline constexpr Color Color::MEDIUM_SLATE_BLUE{0.48235294f, 0.40784314f,
                                                0.93333334f};
inline constexpr Color Color::MEDIUM_SPRING_GREEN{0.0f, 0.98039216f,
                                                  0.6039216f};
inline constexpr Color Color::MEDIUM_TURQUOISE{0.28235295f, 0.81960785f, 0.8f};
inline constexpr Color Color::MEDIUM_VIOLET_RED{0.78039217f, 0.08235294f,
                                                0.52156866f};
inline constexpr Color Color::MIDNIGHT_BLUE{0.09803922f, 0.09803922f,
                                            0.4392157f};
inline constexpr Color Color::MINTCREAM{0.9607843f, 1.0f, 0.98039216f};
inline constexpr Color Color::MISTY_ROSE{1.0f, 0.89411765f, 0.88235295f};
inline constexpr Color Color::MOCCASIN{1.0f, 0.89411765f, 0.70980394f};
inline constexpr Color Color::NAVAJO_WHITE{1.0f, 0.87058824f, 0.6784314f};
inline constexpr Color Color::NAVY{0.0f, 0.0f, 0.5019608f};
inline constexpr Color Color::OLD_LACE{0.99215686f, 0.9607843f, 0.9019608f};
inline constexpr Color Color::OLIVE{0.5019608f, 0.5019608f, 0.0f};
inline constexpr Color Color::OLIVE_DRAB{0.41960785f, 0.5568628f, 0.13725491f};
inline constexpr Color Color::ORANGE{1.0f, 0.64705884f, 0.0f};
inline constexpr Color Color::ORANGE_RED{1.0f, 0.27058825f, 0.0f};
inline constexpr Color Color::ORCHID{0.85490197f, 0.4392157f, 0.8392157f};
inline constexpr Color Color::PALE_GOLDENROD{0.93333334f, 0.9098039f,
                                             0.6666667f};
inline constexpr Color Color::PALE_GREEN{0.59607846f, 0.9843137f, 0.59607846f};
inline constexpr Color Color::PALE_TURQUOISE{0.6862745f, 0.93333334f,
                                             0.93333334f};
inline constexpr Color Color::PALE_VIOLET_RED{0.85882354f, 0.4392157f,
                                              0.5764706f};
inline constexpr Color Color::PAPAYA_WHIP{1.0f, 0.9372549f, 0.8352941f};
inline constexpr Color Color::PEACH_PUFF{1.0f, 0.85490197f, 0.7254902f};
inline constexpr Color Color::PERU{0.8039216f, 0.52156866f, 0.24705882f};
inline constexpr Color Color::PINK{1.0f, 0.7529412f, 0.79607844f};
inline constexpr Color Color::PLUM{0.8666667f, 0.627451f, 0.8666667f};
inline constexpr Color Color::POWDER_BLUE{0.6901961f, 0.8784314f, 0.9019608f};
inline constexpr Color Color::PURPLE{0.5019608f, 0.0f, 0.5019608f};
inline constexpr Color Color::RED{1.0f, 0.0f, 0.0f};
inline constexpr Color Color::ROSY_BROWN{0.7372549f, 0.56078434f, 0.56078434f};
inline constexpr Color Color::ROYAL_BLUE{0.25490198f, 0.4117647f, 0.88235295f};
inline constexpr Color Color::SADDLE_BROWN{0.54509807f, 0.27058825f,
                                           0.07450981f};
inline constexpr Color Color::SALMON{0.98039216f, 0.5019608f, 0.44705883f};
inline constexpr Color Color::SANDY_BROWN{0.95686275f, 0.6431373f, 0.3764706f};
inline constexpr Color Color::SEA_GREEN{0.18039216f, 0.54509807f, 0.34117648f};
inline constexpr Color Color::SEASHELL{1.0f, 0.9607843f, 0.93333334f};
inline constexpr Color Color::SIENNA{0.627451f, 0.32156864f, 0.1764706f};
inline constexpr Color Color::SILVER{0.7529412f, 0.7529412f, 0.7529412f};
inline constexpr Color Color::SKY_BLUE{0.5294118f, 0.80784315f, 0.92156863f};
inline constexpr Color Color::SLATE_BLUE{0.41568628f, 0.3529412f, 0.8039216f};
inline constexpr Color Color::SLATE_GRAY{0.4392157f, 0.5019608f, 0.5647059f};
inline constexpr Color Color::SNOW{1.0f, 0.98039216f, 0.98039216f};
inline constexpr Color Color::SPRING_GREEN{0.0f, 1.0f, 0.49803922f};
inline constexpr Color Color::STEEL_BLUE{0.27450982f, 0.50980395f, 0.7058824f};
inline constexpr Color Color::TAN{0.8235294f, 0.7058824f, 0.54901963f};
inline constexpr Color Color::TEAL{0.0f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::THISTLE{0.84705883f, 0.7490196f, 0.84705883f};
inline constexpr Color Color::TOMATO{1.0f, 0.3882353f, 0.2784314f};
inline constexpr Color Color::TURQUOISE{0.2509804f, 0.8784314f, 0.8156863f};
inline constexpr Color Color::VIOLET{0.93333334f, 0.50980395f, 0.93333334f};
inline constexpr Color Color::WHEAT{0.9607843f, 0.87058824f, 0.7019608f};
inline constexpr Color Color::WHITE{1.0f, 1.0f, 1.0f};
inline constexpr Color Color::WHITE_SMOKE{0.9607843f, 0.9607843f, 0.9607843f};
inline constexpr Color Color::YELLOW{1.0f, 1.0f, 0.0f};
inline constexpr Color Color::YELLOW_GREEN{0.6039216f, 0.8039216f, 0.19607843f};

}  // namespace frc
