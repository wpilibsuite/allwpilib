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
  static const Color kDenim;

  /**
   * 0x0066B3.
   */
  static const Color kFirstBlue;

  /**
   * 0xED1C24.
   */
  static const Color kFirstRed;

  /*
   * Standard Colors
   */

  /**
   * 0xF0F8FF.
   */
  static const Color kAliceBlue;

  /**
   * 0xFAEBD7.
   */
  static const Color kAntiqueWhite;

  /**
   * 0x00FFFF.
   */
  static const Color kAqua;

  /**
   * 0x7FFFD4.
   */
  static const Color kAquamarine;

  /**
   * 0xF0FFFF.
   */
  static const Color kAzure;

  /**
   * 0xF5F5DC.
   */
  static const Color kBeige;

  /**
   * 0xFFE4C4.
   */
  static const Color kBisque;

  /**
   * 0x000000.
   */
  static const Color kBlack;

  /**
   * 0xFFEBCD.
   */
  static const Color kBlanchedAlmond;

  /**
   * 0x0000FF.
   */
  static const Color kBlue;

  /**
   * 0x8A2BE2.
   */
  static const Color kBlueViolet;

  /**
   * 0xA52A2A.
   */
  static const Color kBrown;

  /**
   * 0xDEB887.
   */
  static const Color kBurlywood;

  /**
   * 0x5F9EA0.
   */
  static const Color kCadetBlue;

  /**
   * 0x7FFF00.
   */
  static const Color kChartreuse;

  /**
   * 0xD2691E.
   */
  static const Color kChocolate;

  /**
   * 0xFF7F50.
   */
  static const Color kCoral;

  /**
   * 0x6495ED.
   */
  static const Color kCornflowerBlue;

  /**
   * 0xFFF8DC.
   */
  static const Color kCornsilk;

  /**
   * 0xDC143C.
   */
  static const Color kCrimson;

  /**
   * 0x00FFFF.
   */
  static const Color kCyan;

  /**
   * 0x00008B.
   */
  static const Color kDarkBlue;

  /**
   * 0x008B8B.
   */
  static const Color kDarkCyan;

  /**
   * 0xB8860B.
   */
  static const Color kDarkGoldenrod;

  /**
   * 0xA9A9A9.
   */
  static const Color kDarkGray;

  /**
   * 0x006400.
   */
  static const Color kDarkGreen;

  /**
   * 0xBDB76B.
   */
  static const Color kDarkKhaki;

  /**
   * 0x8B008B.
   */
  static const Color kDarkMagenta;

  /**
   * 0x556B2F.
   */
  static const Color kDarkOliveGreen;

  /**
   * 0xFF8C00.
   */
  static const Color kDarkOrange;

  /**
   * 0x9932CC.
   */
  static const Color kDarkOrchid;

  /**
   * 0x8B0000.
   */
  static const Color kDarkRed;

  /**
   * 0xE9967A.
   */
  static const Color kDarkSalmon;

  /**
   * 0x8FBC8F.
   */
  static const Color kDarkSeaGreen;

  /**
   * 0x483D8B.
   */
  static const Color kDarkSlateBlue;

  /**
   * 0x2F4F4F.
   */
  static const Color kDarkSlateGray;

  /**
   * 0x00CED1.
   */
  static const Color kDarkTurquoise;

  /**
   * 0x9400D3.
   */
  static const Color kDarkViolet;

  /**
   * 0xFF1493.
   */
  static const Color kDeepPink;

  /**
   * 0x00BFFF.
   */
  static const Color kDeepSkyBlue;

  /**
   * 0x696969.
   */
  static const Color kDimGray;

  /**
   * 0x1E90FF.
   */
  static const Color kDodgerBlue;

  /**
   * 0xB22222.
   */
  static const Color kFirebrick;

  /**
   * 0xFFFAF0.
   */
  static const Color kFloralWhite;

  /**
   * 0x228B22.
   */
  static const Color kForestGreen;

  /**
   * 0xFF00FF.
   */
  static const Color kFuchsia;

  /**
   * 0xDCDCDC.
   */
  static const Color kGainsboro;

  /**
   * 0xF8F8FF.
   */
  static const Color kGhostWhite;

  /**
   * 0xFFD700.
   */
  static const Color kGold;

  /**
   * 0xDAA520.
   */
  static const Color kGoldenrod;

  /**
   * 0x808080.
   */
  static const Color kGray;

  /**
   * 0x008000.
   */
  static const Color kGreen;

  /**
   * 0xADFF2F.
   */
  static const Color kGreenYellow;

  /**
   * 0xF0FFF0.
   */
  static const Color kHoneydew;

  /**
   * 0xFF69B4.
   */
  static const Color kHotPink;

  /**
   * 0xCD5C5C.
   */
  static const Color kIndianRed;

  /**
   * 0x4B0082.
   */
  static const Color kIndigo;

  /**
   * 0xFFFFF0.
   */
  static const Color kIvory;

  /**
   * 0xF0E68C.
   */
  static const Color kKhaki;

  /**
   * 0xE6E6FA.
   */
  static const Color kLavender;

  /**
   * 0xFFF0F5.
   */
  static const Color kLavenderBlush;

  /**
   * 0x7CFC00.
   */
  static const Color kLawnGreen;

  /**
   * 0xFFFACD.
   */
  static const Color kLemonChiffon;

  /**
   * 0xADD8E6.
   */
  static const Color kLightBlue;

  /**
   * 0xF08080.
   */
  static const Color kLightCoral;

  /**
   * 0xE0FFFF.
   */
  static const Color kLightCyan;

  /**
   * 0xFAFAD2.
   */
  static const Color kLightGoldenrodYellow;

  /**
   * 0xD3D3D3.
   */
  static const Color kLightGray;

  /**
   * 0x90EE90.
   */
  static const Color kLightGreen;

  /**
   * 0xFFB6C1.
   */
  static const Color kLightPink;

  /**
   * 0xFFA07A.
   */
  static const Color kLightSalmon;

  /**
   * 0x20B2AA.
   */
  static const Color kLightSeaGreen;

  /**
   * 0x87CEFA.
   */
  static const Color kLightSkyBlue;

  /**
   * 0x778899.
   */
  static const Color kLightSlateGray;

  /**
   * 0xB0C4DE.
   */
  static const Color kLightSteelBlue;

  /**
   * 0xFFFFE0.
   */
  static const Color kLightYellow;

  /**
   * 0x00FF00.
   */
  static const Color kLime;

  /**
   * 0x32CD32.
   */
  static const Color kLimeGreen;

  /**
   * 0xFAF0E6.
   */
  static const Color kLinen;

  /**
   * 0xFF00FF.
   */
  static const Color kMagenta;

  /**
   * 0x800000.
   */
  static const Color kMaroon;

  /**
   * 0x66CDAA.
   */
  static const Color kMediumAquamarine;

  /**
   * 0x0000CD.
   */
  static const Color kMediumBlue;

  /**
   * 0xBA55D3.
   */
  static const Color kMediumOrchid;

  /**
   * 0x9370DB.
   */
  static const Color kMediumPurple;

  /**
   * 0x3CB371.
   */
  static const Color kMediumSeaGreen;

  /**
   * 0x7B68EE.
   */
  static const Color kMediumSlateBlue;

  /**
   * 0x00FA9A.
   */
  static const Color kMediumSpringGreen;

  /**
   * 0x48D1CC.
   */
  static const Color kMediumTurquoise;

  /**
   * 0xC71585.
   */
  static const Color kMediumVioletRed;

  /**
   * 0x191970.
   */
  static const Color kMidnightBlue;

  /**
   * 0xF5FFFA.
   */
  static const Color kMintcream;

  /**
   * 0xFFE4E1.
   */
  static const Color kMistyRose;

  /**
   * 0xFFE4B5.
   */
  static const Color kMoccasin;

  /**
   * 0xFFDEAD.
   */
  static const Color kNavajoWhite;

  /**
   * 0x000080.
   */
  static const Color kNavy;

  /**
   * 0xFDF5E6.
   */
  static const Color kOldLace;

  /**
   * 0x808000.
   */
  static const Color kOlive;

  /**
   * 0x6B8E23.
   */
  static const Color kOliveDrab;

  /**
   * 0xFFA500.
   */
  static const Color kOrange;

  /**
   * 0xFF4500.
   */
  static const Color kOrangeRed;

  /**
   * 0xDA70D6.
   */
  static const Color kOrchid;

  /**
   * 0xEEE8AA.
   */
  static const Color kPaleGoldenrod;

  /**
   * 0x98FB98.
   */
  static const Color kPaleGreen;

  /**
   * 0xAFEEEE.
   */
  static const Color kPaleTurquoise;

  /**
   * 0xDB7093.
   */
  static const Color kPaleVioletRed;

  /**
   * 0xFFEFD5.
   */
  static const Color kPapayaWhip;

  /**
   * 0xFFDAB9.
   */
  static const Color kPeachPuff;

  /**
   * 0xCD853F.
   */
  static const Color kPeru;

  /**
   * 0xFFC0CB.
   */
  static const Color kPink;

  /**
   * 0xDDA0DD.
   */
  static const Color kPlum;

  /**
   * 0xB0E0E6.
   */
  static const Color kPowderBlue;

  /**
   * 0x800080.
   */
  static const Color kPurple;

  /**
   * 0xFF0000.
   */
  static const Color kRed;

  /**
   * 0xBC8F8F.
   */
  static const Color kRosyBrown;

  /**
   * 0x4169E1.
   */
  static const Color kRoyalBlue;

  /**
   * 0x8B4513.
   */
  static const Color kSaddleBrown;

  /**
   * 0xFA8072.
   */
  static const Color kSalmon;

  /**
   * 0xF4A460.
   */
  static const Color kSandyBrown;

  /**
   * 0x2E8B57.
   */
  static const Color kSeaGreen;

  /**
   * 0xFFF5EE.
   */
  static const Color kSeashell;

  /**
   * 0xA0522D.
   */
  static const Color kSienna;

  /**
   * 0xC0C0C0.
   */
  static const Color kSilver;

  /**
   * 0x87CEEB.
   */
  static const Color kSkyBlue;

  /**
   * 0x6A5ACD.
   */
  static const Color kSlateBlue;

  /**
   * 0x708090.
   */
  static const Color kSlateGray;

  /**
   * 0xFFFAFA.
   */
  static const Color kSnow;

  /**
   * 0x00FF7F.
   */
  static const Color kSpringGreen;

  /**
   * 0x4682B4.
   */
  static const Color kSteelBlue;

  /**
   * 0xD2B48C.
   */
  static const Color kTan;

  /**
   * 0x008080.
   */
  static const Color kTeal;

  /**
   * 0xD8BFD8.
   */
  static const Color kThistle;

  /**
   * 0xFF6347.
   */
  static const Color kTomato;

  /**
   * 0x40E0D0.
   */
  static const Color kTurquoise;

  /**
   * 0xEE82EE.
   */
  static const Color kViolet;

  /**
   * 0xF5DEB3.
   */
  static const Color kWheat;

  /**
   * 0xFFFFFF.
   */
  static const Color kWhite;

  /**
   * 0xF5F5F5.
   */
  static const Color kWhiteSmoke;

  /**
   * 0xFFFF00.
   */
  static const Color kYellow;

  /**
   * 0x9ACD32.
   */
  static const Color kYellowGreen;

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
inline constexpr Color Color::kDenim{0.0823529412, 0.376470589, 0.7411764706};
inline constexpr Color Color::kFirstBlue{0.0, 0.4, 0.7019607844};
inline constexpr Color Color::kFirstRed{0.9294117648, 0.1098039216,
                                        0.1411764706};

/*
 * Standard Colors
 */
inline constexpr Color Color::kAliceBlue{0.9411765f, 0.972549f, 1.0f};
inline constexpr Color Color::kAntiqueWhite{0.98039216f, 0.92156863f,
                                            0.84313726f};
inline constexpr Color Color::kAqua{0.0f, 1.0f, 1.0f};
inline constexpr Color Color::kAquamarine{0.49803922f, 1.0f, 0.83137256f};
inline constexpr Color Color::kAzure{0.9411765f, 1.0f, 1.0f};
inline constexpr Color Color::kBeige{0.9607843f, 0.9607843f, 0.8627451f};
inline constexpr Color Color::kBisque{1.0f, 0.89411765f, 0.76862746f};
inline constexpr Color Color::kBlack{0.0f, 0.0f, 0.0f};
inline constexpr Color Color::kBlanchedAlmond{1.0f, 0.92156863f, 0.8039216f};
inline constexpr Color Color::kBlue{0.0f, 0.0f, 1.0f};
inline constexpr Color Color::kBlueViolet{0.5411765f, 0.16862746f, 0.8862745f};
inline constexpr Color Color::kBrown{0.64705884f, 0.16470589f, 0.16470589f};
inline constexpr Color Color::kBurlywood{0.87058824f, 0.72156864f, 0.5294118f};
inline constexpr Color Color::kCadetBlue{0.37254903f, 0.61960787f, 0.627451f};
inline constexpr Color Color::kChartreuse{0.49803922f, 1.0f, 0.0f};
inline constexpr Color Color::kChocolate{0.8235294f, 0.4117647f, 0.11764706f};
inline constexpr Color Color::kCoral{1.0f, 0.49803922f, 0.3137255f};
inline constexpr Color Color::kCornflowerBlue{0.39215687f, 0.58431375f,
                                              0.92941177f};
inline constexpr Color Color::kCornsilk{1.0f, 0.972549f, 0.8627451f};
inline constexpr Color Color::kCrimson{0.8627451f, 0.078431375f, 0.23529412f};
inline constexpr Color Color::kCyan{0.0f, 1.0f, 1.0f};
inline constexpr Color Color::kDarkBlue{0.0f, 0.0f, 0.54509807f};
inline constexpr Color Color::kDarkCyan{0.0f, 0.54509807f, 0.54509807f};
inline constexpr Color Color::kDarkGoldenrod{0.72156864f, 0.5254902f,
                                             0.043137256f};
inline constexpr Color Color::kDarkGray{0.6627451f, 0.6627451f, 0.6627451f};
inline constexpr Color Color::kDarkGreen{0.0f, 0.39215687f, 0.0f};
inline constexpr Color Color::kDarkKhaki{0.7411765f, 0.7176471f, 0.41960785f};
inline constexpr Color Color::kDarkMagenta{0.54509807f, 0.0f, 0.54509807f};
inline constexpr Color Color::kDarkOliveGreen{0.33333334f, 0.41960785f,
                                              0.18431373f};
inline constexpr Color Color::kDarkOrange{1.0f, 0.54901963f, 0.0f};
inline constexpr Color Color::kDarkOrchid{0.6f, 0.19607843f, 0.8f};
inline constexpr Color Color::kDarkRed{0.54509807f, 0.0f, 0.0f};
inline constexpr Color Color::kDarkSalmon{0.9137255f, 0.5882353f, 0.47843137f};
inline constexpr Color Color::kDarkSeaGreen{0.56078434f, 0.7372549f,
                                            0.56078434f};
inline constexpr Color Color::kDarkSlateBlue{0.28235295f, 0.23921569f,
                                             0.54509807f};
inline constexpr Color Color::kDarkSlateGray{0.18431373f, 0.30980393f,
                                             0.30980393f};
inline constexpr Color Color::kDarkTurquoise{0.0f, 0.80784315f, 0.81960785f};
inline constexpr Color Color::kDarkViolet{0.5803922f, 0.0f, 0.827451f};
inline constexpr Color Color::kDeepPink{1.0f, 0.078431375f, 0.5764706f};
inline constexpr Color Color::kDeepSkyBlue{0.0f, 0.7490196f, 1.0f};
inline constexpr Color Color::kDimGray{0.4117647f, 0.4117647f, 0.4117647f};
inline constexpr Color Color::kDodgerBlue{0.11764706f, 0.5647059f, 1.0f};
inline constexpr Color Color::kFirebrick{0.69803923f, 0.13333334f, 0.13333334f};
inline constexpr Color Color::kFloralWhite{1.0f, 0.98039216f, 0.9411765f};
inline constexpr Color Color::kForestGreen{0.13333334f, 0.54509807f,
                                           0.13333334f};
inline constexpr Color Color::kFuchsia{1.0f, 0.0f, 1.0f};
inline constexpr Color Color::kGainsboro{0.8627451f, 0.8627451f, 0.8627451f};
inline constexpr Color Color::kGhostWhite{0.972549f, 0.972549f, 1.0f};
inline constexpr Color Color::kGold{1.0f, 0.84313726f, 0.0f};
inline constexpr Color Color::kGoldenrod{0.85490197f, 0.64705884f, 0.1254902f};
inline constexpr Color Color::kGray{0.5019608f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::kGreen{0.0f, 0.5019608f, 0.0f};
inline constexpr Color Color::kGreenYellow{0.6784314f, 1.0f, 0.18431373f};
inline constexpr Color Color::kHoneydew{0.9411765f, 1.0f, 0.9411765f};
inline constexpr Color Color::kHotPink{1.0f, 0.4117647f, 0.7058824f};
inline constexpr Color Color::kIndianRed{0.8039216f, 0.36078432f, 0.36078432f};
inline constexpr Color Color::kIndigo{0.29411766f, 0.0f, 0.50980395f};
inline constexpr Color Color::kIvory{1.0f, 1.0f, 0.9411765f};
inline constexpr Color Color::kKhaki{0.9411765f, 0.9019608f, 0.54901963f};
inline constexpr Color Color::kLavender{0.9019608f, 0.9019608f, 0.98039216f};
inline constexpr Color Color::kLavenderBlush{1.0f, 0.9411765f, 0.9607843f};
inline constexpr Color Color::kLawnGreen{0.4862745f, 0.9882353f, 0.0f};
inline constexpr Color Color::kLemonChiffon{1.0f, 0.98039216f, 0.8039216f};
inline constexpr Color Color::kLightBlue{0.6784314f, 0.84705883f, 0.9019608f};
inline constexpr Color Color::kLightCoral{0.9411765f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::kLightCyan{0.8784314f, 1.0f, 1.0f};
inline constexpr Color Color::kLightGoldenrodYellow{0.98039216f, 0.98039216f,
                                                    0.8235294f};
inline constexpr Color Color::kLightGray{0.827451f, 0.827451f, 0.827451f};
inline constexpr Color Color::kLightGreen{0.5647059f, 0.93333334f, 0.5647059f};
inline constexpr Color Color::kLightPink{1.0f, 0.7137255f, 0.75686276f};
inline constexpr Color Color::kLightSalmon{1.0f, 0.627451f, 0.47843137f};
inline constexpr Color Color::kLightSeaGreen{0.1254902f, 0.69803923f,
                                             0.6666667f};
inline constexpr Color Color::kLightSkyBlue{0.5294118f, 0.80784315f,
                                            0.98039216f};
inline constexpr Color Color::kLightSlateGray{0.46666667f, 0.53333336f, 0.6f};
inline constexpr Color Color::kLightSteelBlue{0.6901961f, 0.76862746f,
                                              0.87058824f};
inline constexpr Color Color::kLightYellow{1.0f, 1.0f, 0.8784314f};
inline constexpr Color Color::kLime{0.0f, 1.0f, 0.0f};
inline constexpr Color Color::kLimeGreen{0.19607843f, 0.8039216f, 0.19607843f};
inline constexpr Color Color::kLinen{0.98039216f, 0.9411765f, 0.9019608f};
inline constexpr Color Color::kMagenta{1.0f, 0.0f, 1.0f};
inline constexpr Color Color::kMaroon{0.5019608f, 0.0f, 0.0f};
inline constexpr Color Color::kMediumAquamarine{0.4f, 0.8039216f, 0.6666667f};
inline constexpr Color Color::kMediumBlue{0.0f, 0.0f, 0.8039216f};
inline constexpr Color Color::kMediumOrchid{0.7294118f, 0.33333334f, 0.827451f};
inline constexpr Color Color::kMediumPurple{0.5764706f, 0.4392157f,
                                            0.85882354f};
inline constexpr Color Color::kMediumSeaGreen{0.23529412f, 0.7019608f,
                                              0.44313726f};
inline constexpr Color Color::kMediumSlateBlue{0.48235294f, 0.40784314f,
                                               0.93333334f};
inline constexpr Color Color::kMediumSpringGreen{0.0f, 0.98039216f, 0.6039216f};
inline constexpr Color Color::kMediumTurquoise{0.28235295f, 0.81960785f, 0.8f};
inline constexpr Color Color::kMediumVioletRed{0.78039217f, 0.08235294f,
                                               0.52156866f};
inline constexpr Color Color::kMidnightBlue{0.09803922f, 0.09803922f,
                                            0.4392157f};
inline constexpr Color Color::kMintcream{0.9607843f, 1.0f, 0.98039216f};
inline constexpr Color Color::kMistyRose{1.0f, 0.89411765f, 0.88235295f};
inline constexpr Color Color::kMoccasin{1.0f, 0.89411765f, 0.70980394f};
inline constexpr Color Color::kNavajoWhite{1.0f, 0.87058824f, 0.6784314f};
inline constexpr Color Color::kNavy{0.0f, 0.0f, 0.5019608f};
inline constexpr Color Color::kOldLace{0.99215686f, 0.9607843f, 0.9019608f};
inline constexpr Color Color::kOlive{0.5019608f, 0.5019608f, 0.0f};
inline constexpr Color Color::kOliveDrab{0.41960785f, 0.5568628f, 0.13725491f};
inline constexpr Color Color::kOrange{1.0f, 0.64705884f, 0.0f};
inline constexpr Color Color::kOrangeRed{1.0f, 0.27058825f, 0.0f};
inline constexpr Color Color::kOrchid{0.85490197f, 0.4392157f, 0.8392157f};
inline constexpr Color Color::kPaleGoldenrod{0.93333334f, 0.9098039f,
                                             0.6666667f};
inline constexpr Color Color::kPaleGreen{0.59607846f, 0.9843137f, 0.59607846f};
inline constexpr Color Color::kPaleTurquoise{0.6862745f, 0.93333334f,
                                             0.93333334f};
inline constexpr Color Color::kPaleVioletRed{0.85882354f, 0.4392157f,
                                             0.5764706f};
inline constexpr Color Color::kPapayaWhip{1.0f, 0.9372549f, 0.8352941f};
inline constexpr Color Color::kPeachPuff{1.0f, 0.85490197f, 0.7254902f};
inline constexpr Color Color::kPeru{0.8039216f, 0.52156866f, 0.24705882f};
inline constexpr Color Color::kPink{1.0f, 0.7529412f, 0.79607844f};
inline constexpr Color Color::kPlum{0.8666667f, 0.627451f, 0.8666667f};
inline constexpr Color Color::kPowderBlue{0.6901961f, 0.8784314f, 0.9019608f};
inline constexpr Color Color::kPurple{0.5019608f, 0.0f, 0.5019608f};
inline constexpr Color Color::kRed{1.0f, 0.0f, 0.0f};
inline constexpr Color Color::kRosyBrown{0.7372549f, 0.56078434f, 0.56078434f};
inline constexpr Color Color::kRoyalBlue{0.25490198f, 0.4117647f, 0.88235295f};
inline constexpr Color Color::kSaddleBrown{0.54509807f, 0.27058825f,
                                           0.07450981f};
inline constexpr Color Color::kSalmon{0.98039216f, 0.5019608f, 0.44705883f};
inline constexpr Color Color::kSandyBrown{0.95686275f, 0.6431373f, 0.3764706f};
inline constexpr Color Color::kSeaGreen{0.18039216f, 0.54509807f, 0.34117648f};
inline constexpr Color Color::kSeashell{1.0f, 0.9607843f, 0.93333334f};
inline constexpr Color Color::kSienna{0.627451f, 0.32156864f, 0.1764706f};
inline constexpr Color Color::kSilver{0.7529412f, 0.7529412f, 0.7529412f};
inline constexpr Color Color::kSkyBlue{0.5294118f, 0.80784315f, 0.92156863f};
inline constexpr Color Color::kSlateBlue{0.41568628f, 0.3529412f, 0.8039216f};
inline constexpr Color Color::kSlateGray{0.4392157f, 0.5019608f, 0.5647059f};
inline constexpr Color Color::kSnow{1.0f, 0.98039216f, 0.98039216f};
inline constexpr Color Color::kSpringGreen{0.0f, 1.0f, 0.49803922f};
inline constexpr Color Color::kSteelBlue{0.27450982f, 0.50980395f, 0.7058824f};
inline constexpr Color Color::kTan{0.8235294f, 0.7058824f, 0.54901963f};
inline constexpr Color Color::kTeal{0.0f, 0.5019608f, 0.5019608f};
inline constexpr Color Color::kThistle{0.84705883f, 0.7490196f, 0.84705883f};
inline constexpr Color Color::kTomato{1.0f, 0.3882353f, 0.2784314f};
inline constexpr Color Color::kTurquoise{0.2509804f, 0.8784314f, 0.8156863f};
inline constexpr Color Color::kViolet{0.93333334f, 0.50980395f, 0.93333334f};
inline constexpr Color Color::kWheat{0.9607843f, 0.87058824f, 0.7019608f};
inline constexpr Color Color::kWhite{1.0f, 1.0f, 1.0f};
inline constexpr Color Color::kWhiteSmoke{0.9607843f, 0.9607843f, 0.9607843f};
inline constexpr Color Color::kYellow{1.0f, 1.0f, 0.0f};
inline constexpr Color Color::kYellowGreen{0.6039216f, 0.8039216f, 0.19607843f};

}  // namespace frc
