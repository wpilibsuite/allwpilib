/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Color8Bit.h"

namespace frc {

class Color8Bit;

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
  static const Color kLightSeagGeen;

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
  static const Color kLightSteellue;

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

  /**
   * Constructs a Color.
   *
   * @param red Red value (0-1)
   * @param green Green value (0-1)
   * @param blue Blue value (0-1)
   */
  constexpr Color(double r, double g, double b) : red(round(r)), green(round(g)), blue(round(b)) {}

  /**
   * Constructs a Color from a Color8Bit.
   *
   * @param color The color
   */
  constexpr Color(Color8Bit color);

  double red;
  double green;
  double blue;

private:
  static constexpr double kPrecision = 1.0 / (1 << 12);

  static constexpr double round(double value) {
    const double rounded = int(value / kPrecision) + 0.5;
    return rounded * kPrecision;
  }
};

bool operator==(const Color& c1, const Color& c2) {
  return c1.red == c2.red &&
      c1.green == c2.green &&
      c1.blue == c2.blue;
}

}  // namespace frc
