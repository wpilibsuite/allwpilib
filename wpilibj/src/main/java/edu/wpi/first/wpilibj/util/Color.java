/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

import java.util.Objects;

import edu.wpi.first.wpiutil.math.MathUtil;

/**
 * Represents colors.
 *
 * <p>Limited to 12 bits of precision.
 */
@SuppressWarnings("MemberName")
public class Color {
  private static final double kPrecision = Math.pow(2, -12);

  public final double red;
  public final double green;
  public final double blue;

  /**
   * Constructs a Color.
   *
   * @param red   Red value (0-1)
   * @param green Green value (0-1)
   * @param blue  Blue value (0-1)
   */
  public Color(double red, double green, double blue) {
    this.red = roundAndClamp(red);
    this.green = roundAndClamp(green);
    this.blue = roundAndClamp(blue);
  }

  /**
   * Constructs a Color from a Color8Bit.
   *
   * @param color The color
   */
  public Color(Color8Bit color) {
    this(color.red / 255.0,
        color.green / 255.0,
        color.blue / 255.0);
  }

  /**
   * Creates a Color from HSV values.
   *
   * @param h The h value [0-180]
   * @param s The s value [0-255]
   * @param v The v value [0-255]
   * @return The color
   */
  @SuppressWarnings("ParameterName")
  public static Color fromHSV(int h, int s, int v) {
    if (s == 0) {
      return new Color(v / 255.0, v / 255.0, v / 255.0);
    }

    final int region = h / 30;
    final int remainder = (h - (region * 30)) * 6;

    final int p = (v * (255 - s)) >> 8;
    final int q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    final int t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
      case 0:
        return new Color(v / 255.0, t / 255.0, p / 255.0);
      case 1:
        return new Color(q / 255.0, v / 255.0, p / 255.0);
      case 2:
        return new Color(p / 255.0, v / 255.0, t / 255.0);
      case 3:
        return new Color(p / 255.0, q / 255.0, v / 255.0);
      case 4:
        return new Color(t / 255.0, p / 255.0, v / 255.0);
      default:
        return new Color(v / 255.0, p / 255.0, q / 255.0);
    }
  }

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null || getClass() != other.getClass()) {
      return false;
    }

    Color color = (Color) other;
    return Double.compare(color.red, red) == 0
        && Double.compare(color.green, green) == 0
        && Double.compare(color.blue, blue) == 0;
  }

  @Override
  public int hashCode() {
    return Objects.hash(red, green, blue);
  }

  private static double roundAndClamp(double value) {
    final var rounded = Math.round((value + kPrecision / 2) / kPrecision) * kPrecision;
    return MathUtil.clamp(rounded, 0.0, 1.0);
  }

  /*
   * FIRST Colors
   */

  /**
   * #1560BD.
   */
  public static final Color kDenim = new Color(0.0823529412, 0.376470589, 0.7411764706);

  /**
   * #0066B3.
   */
  public static final Color kFirstBlue = new Color(0.0, 0.4, 0.7019607844);

  /**
   * #ED1C24.
   */
  public static final Color kFirstRed = new Color(0.9294117648, 0.1098039216, 0.1411764706);

  /*
   * Standard Colors
   */

  /**
   * #F0F8FF.
   */
  public static final Color kAliceBlue = new Color(0.9411765f, 0.972549f, 1.0f);

  /**
   * #FAEBD7.
   */
  public static final Color kAntiqueWhite = new Color(0.98039216f, 0.92156863f, 0.84313726f);

  /**
   * #00FFFF.
   */
  public static final Color kAqua = new Color(0.0f, 1.0f, 1.0f);

  /**
   * #7FFFD4.
    */
  public static final Color kAquamarine = new Color(0.49803922f, 1.0f, 0.83137256f);

  /**
   * #F0FFFF.
   */
  public static final Color kAzure = new Color(0.9411765f, 1.0f, 1.0f);

  /**
   * #F5F5DC.
   */
  public static final Color kBeige = new Color(0.9607843f, 0.9607843f, 0.8627451f);

  /**
   * #FFE4C4.
   */
  public static final Color kBisque = new Color(1.0f, 0.89411765f, 0.76862746f);

  /**
   * #000000.
   */
  public static final Color kBlack = new Color(0.0f, 0.0f, 0.0f);

  /**
   * #FFEBCD.
   */
  public static final Color kBlanchedAlmond = new Color(1.0f, 0.92156863f, 0.8039216f);

  /**
   * #0000FF.
   */
  public static final Color kBlue = new Color(0.0f, 0.0f, 1.0f);

  /**
   * #8A2BE2.
   */
  public static final Color kBlueViolet = new Color(0.5411765f, 0.16862746f, 0.8862745f);

  /**
   * #A52A2A.
   */
  public static final Color kBrown = new Color(0.64705884f, 0.16470589f, 0.16470589f);

  /**
   * #DEB887.
   */
  public static final Color kBurlywood = new Color(0.87058824f, 0.72156864f, 0.5294118f);

  /**
   * #5F9EA0.
   */
  public static final Color kCadetBlue = new Color(0.37254903f, 0.61960787f, 0.627451f);

  /**
   * #7FFF00.
   */
  public static final Color kChartreuse = new Color(0.49803922f, 1.0f, 0.0f);

  /**
   * #D2691E.
   */
  public static final Color kChocolate = new Color(0.8235294f, 0.4117647f, 0.11764706f);

  /**
   * #FF7F50.
   */
  public static final Color kCoral = new Color(1.0f, 0.49803922f, 0.3137255f);

  /**
   * #6495ED.
   */
  public static final Color kCornflowerBlue = new Color(0.39215687f, 0.58431375f, 0.92941177f);

  /**
   * #FFF8DC.
   */
  public static final Color kCornsilk = new Color(1.0f, 0.972549f, 0.8627451f);

  /**
   * #DC143C.
   */
  public static final Color kCrimson = new Color(0.8627451f, 0.078431375f, 0.23529412f);

  /**
   * #00FFFF.
   */
  public static final Color kCyan = new Color(0.0f, 1.0f, 1.0f);

  /**
   * #00008B.
   */
  public static final Color kDarkBlue = new Color(0.0f, 0.0f, 0.54509807f);

  /**
   * #008B8B.
   */
  public static final Color kDarkCyan = new Color(0.0f, 0.54509807f, 0.54509807f);

  /**
   * #B8860B.
   */
  public static final Color kDarkGoldenrod = new Color(0.72156864f, 0.5254902f, 0.043137256f);

  /**
   * #A9A9A9.
   */
  public static final Color kDarkGray = new Color(0.6627451f, 0.6627451f, 0.6627451f);

  /**
   * #006400.
   */
  public static final Color kDarkGreen = new Color(0.0f, 0.39215687f, 0.0f);

  /**
   * #BDB76B.
   */
  public static final Color kDarkKhaki = new Color(0.7411765f, 0.7176471f, 0.41960785f);

  /**
   * #8B008B.
   */
  public static final Color kDarkMagenta = new Color(0.54509807f, 0.0f, 0.54509807f);

  /**
   * #556B2F.
   */
  public static final Color kDarkOliveGreen = new Color(0.33333334f, 0.41960785f, 0.18431373f);

  /**
   * #FF8C00.
   */
  public static final Color kDarkOrange = new Color(1.0f, 0.54901963f, 0.0f);

  /**
   * #9932CC.
   */
  public static final Color kDarkOrchid = new Color(0.6f, 0.19607843f, 0.8f);

  /**
   * #8B0000.
   */
  public static final Color kDarkRed = new Color(0.54509807f, 0.0f, 0.0f);

  /**
   * #E9967A.
   */
  public static final Color kDarkSalmon = new Color(0.9137255f, 0.5882353f, 0.47843137f);

  /**
   * #8FBC8F.
   */
  public static final Color kDarkSeaGreen = new Color(0.56078434f, 0.7372549f, 0.56078434f);

  /**
   * #483D8B.
   */
  public static final Color kDarkSlateBlue = new Color(0.28235295f, 0.23921569f, 0.54509807f);

  /**
   * #2F4F4F.
   */
  public static final Color kDarkSlateGray = new Color(0.18431373f, 0.30980393f, 0.30980393f);

  /**
   * #00CED1.
   */
  public static final Color kDarkTurquoise = new Color(0.0f, 0.80784315f, 0.81960785f);

  /**
   * #9400D3.
   */
  public static final Color kDarkViolet = new Color(0.5803922f, 0.0f, 0.827451f);

  /**
   * #FF1493.
   */
  public static final Color kDeepPink = new Color(1.0f, 0.078431375f, 0.5764706f);

  /**
   * #00BFFF.
   */
  public static final Color kDeepSkyBlue = new Color(0.0f, 0.7490196f, 1.0f);

  /**
   * #696969.
   */
  public static final Color kDimGray = new Color(0.4117647f, 0.4117647f, 0.4117647f);

  /**
   * #1E90FF.
   */
  public static final Color kDodgerBlue = new Color(0.11764706f, 0.5647059f, 1.0f);

  /**
   * #B22222.
   */
  public static final Color kFirebrick = new Color(0.69803923f, 0.13333334f, 0.13333334f);

  /**
   * #FFFAF0.
   */
  public static final Color kFloralWhite = new Color(1.0f, 0.98039216f, 0.9411765f);

  /**
   * #228B22.
   */
  public static final Color kForestGreen = new Color(0.13333334f, 0.54509807f, 0.13333334f);

  /**
   * #FF00FF.
   */
  public static final Color kFuchsia = new Color(1.0f, 0.0f, 1.0f);

  /**
   * #DCDCDC.
   */
  public static final Color kGainsboro = new Color(0.8627451f, 0.8627451f, 0.8627451f);

  /**
   * #F8F8FF.
   */
  public static final Color kGhostWhite = new Color(0.972549f, 0.972549f, 1.0f);

  /**
   * #FFD700.
   */
  public static final Color kGold = new Color(1.0f, 0.84313726f, 0.0f);

  /**
   * #DAA520.
   */
  public static final Color kGoldenrod = new Color(0.85490197f, 0.64705884f, 0.1254902f);

  /**
   * #808080.
   */
  public static final Color kGray = new Color(0.5019608f, 0.5019608f, 0.5019608f);

  /**
   * #008000.
   */
  public static final Color kGreen = new Color(0.0f, 0.5019608f, 0.0f);

  /**
   * #ADFF2F.
   */
  public static final Color kGreenYellow = new Color(0.6784314f, 1.0f, 0.18431373f);

  /**
   * #F0FFF0.
   */
  public static final Color kHoneydew = new Color(0.9411765f, 1.0f, 0.9411765f);

  /**
   * #FF69B4.
   */
  public static final Color kHotPink = new Color(1.0f, 0.4117647f, 0.7058824f);

  /**
   * #CD5C5C.
   */
  public static final Color kIndianRed = new Color(0.8039216f, 0.36078432f, 0.36078432f);

  /**
   * #4B0082.
   */
  public static final Color kIndigo = new Color(0.29411766f, 0.0f, 0.50980395f);

  /**
   * #FFFFF0.
   */
  public static final Color kIvory = new Color(1.0f, 1.0f, 0.9411765f);

  /**
   * #F0E68C.
   */
  public static final Color kKhaki = new Color(0.9411765f, 0.9019608f, 0.54901963f);

  /**
   * #E6E6FA.
   */
  public static final Color kLavender = new Color(0.9019608f, 0.9019608f, 0.98039216f);

  /**
   * #FFF0F5.
   */
  public static final Color kLavenderBlush = new Color(1.0f, 0.9411765f, 0.9607843f);

  /**
   * #7CFC00.
   */
  public static final Color kLawnGreen = new Color(0.4862745f, 0.9882353f, 0.0f);

  /**
   * #FFFACD.
   */
  public static final Color kLemonChiffon = new Color(1.0f, 0.98039216f, 0.8039216f);

  /**
   * #ADD8E6.
   */
  public static final Color kLightBlue = new Color(0.6784314f, 0.84705883f, 0.9019608f);

  /**
   * #F08080.
   */
  public static final Color kLightCoral = new Color(0.9411765f, 0.5019608f, 0.5019608f);

  /**
   * #E0FFFF.
   */
  public static final Color kLightCyan = new Color(0.8784314f, 1.0f, 1.0f);

  /**
   * #FAFAD2.
   */
  public static final Color kLightGoldenrodYellow = new Color(0.98039216f, 0.98039216f, 0.8235294f);

  /**
   * #D3D3D3.
   */
  public static final Color kLightGray = new Color(0.827451f, 0.827451f, 0.827451f);

  /**
   * #90EE90.
   */
  public static final Color kLightGreen = new Color(0.5647059f, 0.93333334f, 0.5647059f);

  /**
   * #FFB6C1.
   */
  public static final Color kLightPink = new Color(1.0f, 0.7137255f, 0.75686276f);

  /**
   * #FFA07A.
   */
  public static final Color kLightSalmon = new Color(1.0f, 0.627451f, 0.47843137f);

  /**
   * #20B2AA.
   */
  public static final Color kLightSeaGreen = new Color(0.1254902f, 0.69803923f, 0.6666667f);

  /**
   * #87CEFA.
   */
  public static final Color kLightSkyBlue = new Color(0.5294118f, 0.80784315f, 0.98039216f);

  /**
   * #778899.
   */
  public static final Color kLightSlateGray = new Color(0.46666667f, 0.53333336f, 0.6f);

  /**
   * #B0C4DE.
   */
  public static final Color kLightSteelBlue = new Color(0.6901961f, 0.76862746f, 0.87058824f);

  /**
   * #FFFFE0.
   */
  public static final Color kLightYellow = new Color(1.0f, 1.0f, 0.8784314f);

  /**
   * #00FF00.
   */
  public static final Color kLime = new Color(0.0f, 1.0f, 0.0f);

  /**
   * #32CD32.
   */
  public static final Color kLimeGreen = new Color(0.19607843f, 0.8039216f, 0.19607843f);

  /**
   * #FAF0E6.
   */
  public static final Color kLinen = new Color(0.98039216f, 0.9411765f, 0.9019608f);

  /**
   * #FF00FF.
   */
  public static final Color kMagenta = new Color(1.0f, 0.0f, 1.0f);

  /**
   * #800000.
   */
  public static final Color kMaroon = new Color(0.5019608f, 0.0f, 0.0f);

  /**
   * #66CDAA.
   */
  public static final Color kMediumAquamarine = new Color(0.4f, 0.8039216f, 0.6666667f);

  /**
   * #0000CD.
   */
  public static final Color kMediumBlue = new Color(0.0f, 0.0f, 0.8039216f);

  /**
   * #BA55D3.
   */
  public static final Color kMediumOrchid = new Color(0.7294118f, 0.33333334f, 0.827451f);

  /**
   * #9370DB.
   */
  public static final Color kMediumPurple = new Color(0.5764706f, 0.4392157f, 0.85882354f);

  /**
   * #3CB371.
   */
  public static final Color kMediumSeaGreen = new Color(0.23529412f, 0.7019608f, 0.44313726f);

  /**
   * #7B68EE.
   */
  public static final Color kMediumSlateBlue = new Color(0.48235294f, 0.40784314f, 0.93333334f);

  /**
   * #00FA9A.
   */
  public static final Color kMediumSpringGreen = new Color(0.0f, 0.98039216f, 0.6039216f);

  /**
   * #48D1CC.
   */
  public static final Color kMediumTurquoise = new Color(0.28235295f, 0.81960785f, 0.8f);

  /**
   * #C71585.
   */
  public static final Color kMediumVioletRed = new Color(0.78039217f, 0.08235294f, 0.52156866f);

  /**
   * #191970.
   */
  public static final Color kMidnightBlue = new Color(0.09803922f, 0.09803922f, 0.4392157f);

  /**
   * #F5FFFA.
   */
  public static final Color kMintcream = new Color(0.9607843f, 1.0f, 0.98039216f);

  /**
   * #FFE4E1.
   */
  public static final Color kMistyRose = new Color(1.0f, 0.89411765f, 0.88235295f);

  /**
   * #FFE4B5.
   */
  public static final Color kMoccasin = new Color(1.0f, 0.89411765f, 0.70980394f);

  /**
   * #FFDEAD.
   */
  public static final Color kNavajoWhite = new Color(1.0f, 0.87058824f, 0.6784314f);

  /**
   * #000080.
   */
  public static final Color kNavy = new Color(0.0f, 0.0f, 0.5019608f);

  /**
   * #FDF5E6.
   */
  public static final Color kOldLace = new Color(0.99215686f, 0.9607843f, 0.9019608f);

  /**
   * #808000.
   */
  public static final Color kOlive = new Color(0.5019608f, 0.5019608f, 0.0f);

  /**
   * #6B8E23.
   */
  public static final Color kOliveDrab = new Color(0.41960785f, 0.5568628f, 0.13725491f);

  /**
   * #FFA500.
   */
  public static final Color kOrange = new Color(1.0f, 0.64705884f, 0.0f);

  /**
   * #FF4500.
   */
  public static final Color kOrangeRed = new Color(1.0f, 0.27058825f, 0.0f);

  /**
   * #DA70D6.
   */
  public static final Color kOrchid = new Color(0.85490197f, 0.4392157f, 0.8392157f);

  /**
   * #EEE8AA.
   */
  public static final Color kPaleGoldenrod = new Color(0.93333334f, 0.9098039f, 0.6666667f);

  /**
   * #98FB98.
   */
  public static final Color kPaleGreen = new Color(0.59607846f, 0.9843137f, 0.59607846f);

  /**
   * #AFEEEE.
   */
  public static final Color kPaleTurquoise = new Color(0.6862745f, 0.93333334f, 0.93333334f);

  /**
   * #DB7093.
   */
  public static final Color kPaleVioletRed = new Color(0.85882354f, 0.4392157f, 0.5764706f);

  /**
   * #FFEFD5.
   */
  public static final Color kPapayaWhip = new Color(1.0f, 0.9372549f, 0.8352941f);

  /**
   * #FFDAB9.
   */
  public static final Color kPeachPuff = new Color(1.0f, 0.85490197f, 0.7254902f);

  /**
   * #CD853F.
   */
  public static final Color kPeru = new Color(0.8039216f, 0.52156866f, 0.24705882f);

  /**
   * #FFC0CB.
   */
  public static final Color kPink = new Color(1.0f, 0.7529412f, 0.79607844f);

  /**
   * #DDA0DD.
   */
  public static final Color kPlum = new Color(0.8666667f, 0.627451f, 0.8666667f);

  /**
   * #B0E0E6.
   */
  public static final Color kPowderBlue = new Color(0.6901961f, 0.8784314f, 0.9019608f);

  /**
   * #800080.
   */
  public static final Color kPurple = new Color(0.5019608f, 0.0f, 0.5019608f);

  /**
   * #FF0000.
   */
  public static final Color kRed = new Color(1.0f, 0.0f, 0.0f);

  /**
   * #BC8F8F.
   */
  public static final Color kRosyBrown = new Color(0.7372549f, 0.56078434f, 0.56078434f);

  /**
   * #4169E1.
   */
  public static final Color kRoyalBlue = new Color(0.25490198f, 0.4117647f, 0.88235295f);

  /**
   * #8B4513.
   */
  public static final Color kSaddleBrown = new Color(0.54509807f, 0.27058825f, 0.07450981f);

  /**
   * #FA8072.
   */
  public static final Color kSalmon = new Color(0.98039216f, 0.5019608f, 0.44705883f);

  /**
   * #F4A460.
   */
  public static final Color kSandyBrown = new Color(0.95686275f, 0.6431373f, 0.3764706f);

  /**
   * #2E8B57.
   */
  public static final Color kSeaGreen = new Color(0.18039216f, 0.54509807f, 0.34117648f);

  /**
   * #FFF5EE.
   */
  public static final Color kSeashell = new Color(1.0f, 0.9607843f, 0.93333334f);

  /**
   * #A0522D.
   */
  public static final Color kSienna = new Color(0.627451f, 0.32156864f, 0.1764706f);

  /**
   * #C0C0C0.
   */
  public static final Color kSilver = new Color(0.7529412f, 0.7529412f, 0.7529412f);

  /**
   * #87CEEB.
   */
  public static final Color kSkyBlue = new Color(0.5294118f, 0.80784315f, 0.92156863f);

  /**
   * #6A5ACD.
   */
  public static final Color kSlateBlue = new Color(0.41568628f, 0.3529412f, 0.8039216f);

  /**
   * #708090.
   */
  public static final Color kSlateGray = new Color(0.4392157f, 0.5019608f, 0.5647059f);

  /**
   * #FFFAFA.
   */
  public static final Color kSnow = new Color(1.0f, 0.98039216f, 0.98039216f);

  /**
   * #00FF7F.
   */
  public static final Color kSpringGreen = new Color(0.0f, 1.0f, 0.49803922f);

  /**
   * #4682B4.
   */
  public static final Color kSteelBlue = new Color(0.27450982f, 0.50980395f, 0.7058824f);

  /**
   * #D2B48C.
   */
  public static final Color kTan = new Color(0.8235294f, 0.7058824f, 0.54901963f);

  /**
   * #008080.
   */
  public static final Color kTeal = new Color(0.0f, 0.5019608f, 0.5019608f);

  /**
   * #D8BFD8.
   */
  public static final Color kThistle = new Color(0.84705883f, 0.7490196f, 0.84705883f);

  /**
   * #FF6347.
   */
  public static final Color kTomato = new Color(1.0f, 0.3882353f, 0.2784314f);

  /**
   * #40E0D0.
   */
  public static final Color kTurquoise = new Color(0.2509804f, 0.8784314f, 0.8156863f);

  /**
   * #EE82EE.
   */
  public static final Color kViolet = new Color(0.93333334f, 0.50980395f, 0.93333334f);

  /**
   * #F5DEB3.
   */
  public static final Color kWheat = new Color(0.9607843f, 0.87058824f, 0.7019608f);

  /**
   * #FFFFFF.
   */
  public static final Color kWhite = new Color(1.0f, 1.0f, 1.0f);

  /**
   * #F5F5F5.
   */
  public static final Color kWhiteSmoke = new Color(0.9607843f, 0.9607843f, 0.9607843f);

  /**
   * #FFFF00.
   */
  public static final Color kYellow = new Color(1.0f, 1.0f, 0.0f);

  /**
   * #9ACD32.
   */
  public static final Color kYellowGreen = new Color(0.6039216f, 0.8039216f, 0.19607843f);
}
