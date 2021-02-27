// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>

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
   * #1560BD.
   */
  static const Color kDenim;

  /**
   * #0066B3.
   */
  static const Color kFirstBlue;

  /**
   * #ED1C24.
   */
  static const Color kFirstRed;

  /*
   * Standard Colors
   */

  /**
   * #F0F8FF.
   */
  static const Color kAliceBlue;

  /**
   * #FAEBD7.
   */
  static const Color kAntiqueWhite;

  /**
   * #00FFFF.
   */
  static const Color kAqua;

  /**
   * #7FFFD4.
   */
  static const Color kAquamarine;

  /**
   * #F0FFFF.
   */
  static const Color kAzure;

  /**
   * #F5F5DC.
   */
  static const Color kBeige;

  /**
   * #FFE4C4.
   */
  static const Color kBisque;

  /**
   * #000000.
   */
  static const Color kBlack;

  /**
   * #FFEBCD.
   */
  static const Color kBlanchedAlmond;

  /**
   * #0000FF.
   */
  static const Color kBlue;

  /**
   * #8A2BE2.
   */
  static const Color kBlueViolet;

  /**
   * #A52A2A.
   */
  static const Color kBrown;

  /**
   * #DEB887.
   */
  static const Color kBurlywood;

  /**
   * #5F9EA0.
   */
  static const Color kCadetBlue;

  /**
   * #7FFF00.
   */
  static const Color kChartreuse;

  /**
   * #D2691E.
   */
  static const Color kChocolate;

  /**
   * #FF7F50.
   */
  static const Color kCoral;

  /**
   * #6495ED.
   */
  static const Color kCornflowerBlue;

  /**
   * #FFF8DC.
   */
  static const Color kCornsilk;

  /**
   * #DC143C.
   */
  static const Color kCrimson;

  /**
   * #00FFFF.
   */
  static const Color kCyan;

  /**
   * #00008B.
   */
  static const Color kDarkBlue;

  /**
   * #008B8B.
   */
  static const Color kDarkCyan;

  /**
   * #B8860B.
   */
  static const Color kDarkGoldenrod;

  /**
   * #A9A9A9.
   */
  static const Color kDarkGray;

  /**
   * #006400.
   */
  static const Color kDarkGreen;

  /**
   * #BDB76B.
   */
  static const Color kDarkKhaki;

  /**
   * #8B008B.
   */
  static const Color kDarkMagenta;

  /**
   * #556B2F.
   */
  static const Color kDarkOliveGreen;

  /**
   * #FF8C00.
   */
  static const Color kDarkOrange;

  /**
   * #9932CC.
   */
  static const Color kDarkOrchid;

  /**
   * #8B0000.
   */
  static const Color kDarkRed;

  /**
   * #E9967A.
   */
  static const Color kDarkSalmon;

  /**
   * #8FBC8F.
   */
  static const Color kDarkSeaGreen;

  /**
   * #483D8B.
   */
  static const Color kDarkSlateBlue;

  /**
   * #2F4F4F.
   */
  static const Color kDarkSlateGray;

  /**
   * #00CED1.
   */
  static const Color kDarkTurquoise;

  /**
   * #9400D3.
   */
  static const Color kDarkViolet;

  /**
   * #FF1493.
   */
  static const Color kDeepPink;

  /**
   * #00BFFF.
   */
  static const Color kDeepSkyBlue;

  /**
   * #696969.
   */
  static const Color kDimGray;

  /**
   * #1E90FF.
   */
  static const Color kDodgerBlue;

  /**
   * #B22222.
   */
  static const Color kFirebrick;

  /**
   * #FFFAF0.
   */
  static const Color kFloralWhite;

  /**
   * #228B22.
   */
  static const Color kForestGreen;

  /**
   * #FF00FF.
   */
  static const Color kFuchsia;

  /**
   * #DCDCDC.
   */
  static const Color kGainsboro;

  /**
   * #F8F8FF.
   */
  static const Color kGhostWhite;

  /**
   * #FFD700.
   */
  static const Color kGold;

  /**
   * #DAA520.
   */
  static const Color kGoldenrod;

  /**
   * #808080.
   */
  static const Color kGray;

  /**
   * #008000.
   */
  static const Color kGreen;

  /**
   * #ADFF2F.
   */
  static const Color kGreenYellow;

  /**
   * #F0FFF0.
   */
  static const Color kHoneydew;

  /**
   * #FF69B4.
   */
  static const Color kHotPink;

  /**
   * #CD5C5C.
   */
  static const Color kIndianRed;

  /**
   * #4B0082.
   */
  static const Color kIndigo;

  /**
   * #FFFFF0.
   */
  static const Color kIvory;

  /**
   * #F0E68C.
   */
  static const Color kKhaki;

  /**
   * #E6E6FA.
   */
  static const Color kLavender;

  /**
   * #FFF0F5.
   */
  static const Color kLavenderBlush;

  /**
   * #7CFC00.
   */
  static const Color kLawnGreen;

  /**
   * #FFFACD.
   */
  static const Color kLemonChiffon;

  /**
   * #ADD8E6.
   */
  static const Color kLightBlue;

  /**
   * #F08080.
   */
  static const Color kLightCoral;

  /**
   * #E0FFFF.
   */
  static const Color kLightCyan;

  /**
   * #FAFAD2.
   */
  static const Color kLightGoldenrodYellow;

  /**
   * #D3D3D3.
   */
  static const Color kLightGray;

  /**
   * #90EE90.
   */
  static const Color kLightGreen;

  /**
   * #FFB6C1.
   */
  static const Color kLightPink;

  /**
   * #FFA07A.
   */
  static const Color kLightSalmon;

  /**
   * #20B2AA.
   */
  static const Color kLightSeaGreen;

  /**
   * #87CEFA.
   */
  static const Color kLightSkyBlue;

  /**
   * #778899.
   */
  static const Color kLightSlateGray;

  /**
   * #B0C4DE.
   */
  static const Color kLightSteelBlue;

  /**
   * #FFFFE0.
   */
  static const Color kLightYellow;

  /**
   * #00FF00.
   */
  static const Color kLime;

  /**
   * #32CD32.
   */
  static const Color kLimeGreen;

  /**
   * #FAF0E6.
   */
  static const Color kLinen;

  /**
   * #FF00FF.
   */
  static const Color kMagenta;

  /**
   * #800000.
   */
  static const Color kMaroon;

  /**
   * #66CDAA.
   */
  static const Color kMediumAquamarine;

  /**
   * #0000CD.
   */
  static const Color kMediumBlue;

  /**
   * #BA55D3.
   */
  static const Color kMediumOrchid;

  /**
   * #9370DB.
   */
  static const Color kMediumPurple;

  /**
   * #3CB371.
   */
  static const Color kMediumSeaGreen;

  /**
   * #7B68EE.
   */
  static const Color kMediumSlateBlue;

  /**
   * #00FA9A.
   */
  static const Color kMediumSpringGreen;

  /**
   * #48D1CC.
   */
  static const Color kMediumTurquoise;

  /**
   * #C71585.
   */
  static const Color kMediumVioletRed;

  /**
   * #191970.
   */
  static const Color kMidnightBlue;

  /**
   * #F5FFFA.
   */
  static const Color kMintcream;

  /**
   * #FFE4E1.
   */
  static const Color kMistyRose;

  /**
   * #FFE4B5.
   */
  static const Color kMoccasin;

  /**
   * #FFDEAD.
   */
  static const Color kNavajoWhite;

  /**
   * #000080.
   */
  static const Color kNavy;

  /**
   * #FDF5E6.
   */
  static const Color kOldLace;

  /**
   * #808000.
   */
  static const Color kOlive;

  /**
   * #6B8E23.
   */
  static const Color kOliveDrab;

  /**
   * #FFA500.
   */
  static const Color kOrange;

  /**
   * #FF4500.
   */
  static const Color kOrangeRed;

  /**
   * #DA70D6.
   */
  static const Color kOrchid;

  /**
   * #EEE8AA.
   */
  static const Color kPaleGoldenrod;

  /**
   * #98FB98.
   */
  static const Color kPaleGreen;

  /**
   * #AFEEEE.
   */
  static const Color kPaleTurquoise;

  /**
   * #DB7093.
   */
  static const Color kPaleVioletRed;

  /**
   * #FFEFD5.
   */
  static const Color kPapayaWhip;

  /**
   * #FFDAB9.
   */
  static const Color kPeachPuff;

  /**
   * #CD853F.
   */
  static const Color kPeru;

  /**
   * #FFC0CB.
   */
  static const Color kPink;

  /**
   * #DDA0DD.
   */
  static const Color kPlum;

  /**
   * #B0E0E6.
   */
  static const Color kPowderBlue;

  /**
   * #800080.
   */
  static const Color kPurple;

  /**
   * #FF0000.
   */
  static const Color kRed;

  /**
   * #BC8F8F.
   */
  static const Color kRosyBrown;

  /**
   * #4169E1.
   */
  static const Color kRoyalBlue;

  /**
   * #8B4513.
   */
  static const Color kSaddleBrown;

  /**
   * #FA8072.
   */
  static const Color kSalmon;

  /**
   * #F4A460.
   */
  static const Color kSandyBrown;

  /**
   * #2E8B57.
   */
  static const Color kSeaGreen;

  /**
   * #FFF5EE.
   */
  static const Color kSeashell;

  /**
   * #A0522D.
   */
  static const Color kSienna;

  /**
   * #C0C0C0.
   */
  static const Color kSilver;

  /**
   * #87CEEB.
   */
  static const Color kSkyBlue;

  /**
   * #6A5ACD.
   */
  static const Color kSlateBlue;

  /**
   * #708090.
   */
  static const Color kSlateGray;

  /**
   * #FFFAFA.
   */
  static const Color kSnow;

  /**
   * #00FF7F.
   */
  static const Color kSpringGreen;

  /**
   * #4682B4.
   */
  static const Color kSteelBlue;

  /**
   * #D2B48C.
   */
  static const Color kTan;

  /**
   * #008080.
   */
  static const Color kTeal;

  /**
   * #D8BFD8.
   */
  static const Color kThistle;

  /**
   * #FF6347.
   */
  static const Color kTomato;

  /**
   * #40E0D0.
   */
  static const Color kTurquoise;

  /**
   * #EE82EE.
   */
  static const Color kViolet;

  /**
   * #F5DEB3.
   */
  static const Color kWheat;

  /**
   * #FFFFFF.
   */
  static const Color kWhite;

  /**
   * #F5F5F5.
   */
  static const Color kWhiteSmoke;

  /**
   * #FFFF00.
   */
  static const Color kYellow;

  /**
   * #9ACD32.
   */
  static const Color kYellowGreen;

  constexpr Color() = default;

  /**
   * Constructs a Color.
   *
   * @param red Red value (0-1)
   * @param green Green value (0-1)
   * @param blue Blue value (0-1)
   */
  constexpr Color(double r, double g, double b)
      : red(roundAndClamp(r)),
        green(roundAndClamp(g)),
        blue(roundAndClamp(b)) {}

  /**
   * Creates a Color from HSV values.
   *
   * @param h The h value [0-180]
   * @param s The s value [0-255]
   * @param v The v value [0-255]
   * @return The color
   */
  static constexpr Color FromHSV(int h, int s, int v) {
    if (s == 0) {
      return {v / 255.0, v / 255.0, v / 255.0};
    }

    int region = h / 30;
    int remainder = (h - (region * 30)) * 6;

    int p = (v * (255 - s)) >> 8;
    int q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    int t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
      case 0:
        return Color(v / 255.0, t / 255.0, p / 255.0);
      case 1:
        return Color(q / 255.0, v / 255.0, p / 255.0);
      case 2:
        return Color(p / 255.0, v / 255.0, t / 255.0);
      case 3:
        return Color(p / 255.0, q / 255.0, v / 255.0);
      case 4:
        return Color(t / 255.0, p / 255.0, v / 255.0);
      default:
        return Color(v / 255.0, p / 255.0, q / 255.0);
    }
  }

  double red = 0.0;
  double green = 0.0;
  double blue = 0.0;

 private:
  static constexpr double kPrecision = 1.0 / (1 << 12);

  static constexpr double roundAndClamp(double value) {
    const auto rounded =
        (static_cast<int>(value / kPrecision) + 0.5) * kPrecision;
    return std::clamp(rounded, 0.0, 1.0);
  }
};

inline bool operator==(const Color& c1, const Color& c2) {
  return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue;
}

inline bool operator!=(const Color& c1, const Color& c2) {
  return !(c1 == c2);
}

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
