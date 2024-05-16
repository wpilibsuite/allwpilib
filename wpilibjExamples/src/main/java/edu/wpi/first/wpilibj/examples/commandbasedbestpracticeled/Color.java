// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.wpilibj.DriverStation;
import java.util.Objects;

/**
 * Represents colors.
 *
 * <p>Limited to 12 bits of precision.
 */
@SuppressWarnings("MemberName")
public class Color {
  /** Red component (0-1). */
  public final double red;

  /** Green component (0-1). */
  public final double green;

  /** Blue component (0-1). */
  public final double blue;

  private String m_name;

  /** Constructs a default color (black). */
  public Color() {
    red = 0.0;
    green = 0.0;
    blue = 0.0;
  }

  /**
   * Constructs a Color from doubles.
   *
   * @param red Red value (0-1)
   * @param green Green value (0-1)
   * @param blue Blue value (0-1)
   */
  public Color(double red, double green, double blue) {
    this.red = roundAndClamp(red);
    this.green = roundAndClamp(green);
    this.blue = roundAndClamp(blue);
    this.m_name = null;
  }

  /**
   * Constructs a Color from ints.
   *
   * @param red Red value (0-255)
   * @param green Green value (0-255)
   * @param blue Blue value (0-255)
   */
  public Color(int red, int green, int blue) {
    this(red / 255.0, green / 255.0, blue / 255.0);
  }

  /**
   * Constructs a Color from a Color8Bit.
   *
   * @param color The color
   */
  public Color(Color8Bit color) {
    this(color.red / 255.0, color.green / 255.0, color.blue / 255.0);
  }

  /**
   * Constructs a Color from doubles.
   *
   * @param red Red value (0-1)
   * @param green Green value (0-1)
   * @param blue Blue value (0-1)
   */
  private Color(double red, double green, double blue, String name) {
    this.red = roundAndClamp(red);
    this.green = roundAndClamp(green);
    this.blue = roundAndClamp(blue);
    this.m_name = name;
  }

  /**
   * Constructs a Color from a hex string.
   *
   * @param hexString a string of the format <code>#RRGGBB</code>
   * @throws IllegalArgumentException if the hex string is invalid.
   */
  public Color(String hexString) {
    if (hexString.length() != 7 || !hexString.startsWith("#")) {
      throw new IllegalArgumentException("Invalid hex string \"" + hexString + "\"");
    }

    this.red = Integer.valueOf(hexString.substring(1, 3), 16) / 255.0;
    this.green = Integer.valueOf(hexString.substring(3, 5), 16) / 255.0;
    this.blue = Integer.valueOf(hexString.substring(5, 7), 16) / 255.0;
  }

  /**
   * Creates a Color from HSV values.
   *
   * @param h The h value [0-180)
   * @param s The s value [0-255]
   * @param v The v value [0-255]
   * @return The color
   */
  public static Color fromHSV(int h, int s, int v) {
    int rgb = hsvToRgb(h, s, v);
    return new Color(
        unpackRGB(rgb, RGBChannel.kRed),
        unpackRGB(rgb, RGBChannel.kGreen),
        unpackRGB(rgb, RGBChannel.kBlue));
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

  @Override
  public String toString() {
    if (m_name == null) {
      // cache hex conversion
      m_name = toHexString();
    }
    return m_name;
  }

  /**
   * Return this color represented as a hex string.
   *
   * @return a string of the format <code>#RRGGBB</code>
   */
  public String toHexString() {
    return String.format(
        "#%02X%02X%02X", (int) (red * 255), (int) (green * 255), (int) (blue * 255));
  }

  private static double roundAndClamp(double value) {
    return MathUtil.clamp(Math.ceil(value * (1 << 12)) / (1 << 12), 0.0, 1.0);
  }

  // Helper methods

  /**
   * Converts HSV values to RGB values. The returned RGB color is packed into a 32-bit integer for
   * memory performance reasons.
   *
   * @param h The h value [0-180)
   * @param s The s value [0-255]
   * @param v The v value [0-255]
   * @return the packed RGB color
   */
  public static int hsvToRgb(int h, int s, int v) {
    // Loosely based on
    // https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
    // The hue range is split into 60 degree regions where in each region there
    // is one rgb component at a low value (m), one at a high value (v) and one
    // that changes (X) from low to high (X+m) or high to low (v-X)

    // Difference between highest and lowest value of any rgb component
    final int chroma = (s * v) / 255;

    // Because hue is 0-180 rather than 0-360 use 30 not 60
    final int region = (h / 30) % 6;

    // Remainder converted from 0-30 to 0-255
    final int remainder = (int) Math.round((h % 30) * (255 / 30.0));

    // Value of the lowest rgb component
    final int m = v - chroma;

    // Goes from 0 to chroma as hue increases
    final int X = (chroma * remainder) >> 8;

    int red;
    int green;
    int blue;
    switch (region) {
      case 0:
        red = v;
        green = X + m;
        blue = m;
        break;
      case 1:
        red = v - X;
        green = v;
        blue = m;
        break;
      case 2:
        red = m;
        green = v;
        blue = X + m;
        break;
      case 3:
        red = m;
        green = v - X;
        blue = v;
        break;
      case 4:
        red = X + m;
        green = m;
        blue = v;
        break;
      default:
        red = v;
        green = m;
        blue = v - X;
        break;
    }
    return packRGB(red, green, blue);
  }

  /** Represents a color channel in an RGB color. */
  public enum RGBChannel {
    /** The red channel of an RGB color. */
    kRed,
    /** The green channel of an RGB color. */
    kGreen,
    /** The blue channel of an RGB color. */
    kBlue
  }

  /**
   * Packs 3 RGB values into a single 32-bit integer. These values can be unpacked with
   * {@link #unpackRGB(int, RGBChannel)} to retrieve the values. This is helpful for avoiding memory
   * allocations of new {@code Color} objects and its resulting garbage collector pressure.
   *
   * @param r the value of the first channel to pack
   * @param g the value of the second channel to pack
   * @param b the value of the third channel to pack
   * @return the packed integer
   */
  public static int packRGB(int r, int g, int b) {
    return (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF);
  }

  /**
   * Unpacks a single color channel from a packed 32-bit RGB integer.
   *
   * <p>Note: Packed RGB colors are expected to be in byte order [empty][red][green][blue].
   *
   * @param packedColor the packed color to extract from
   * @param channel the color channel to unpack
   * @return the value of the stored color channel
   */
  public static int unpackRGB(int packedColor, RGBChannel channel) {
    switch (channel) {
      case kRed:
        return (packedColor >> 16) & 0xFF;
      case kGreen:
        return (packedColor >> 8) & 0xFF;
      case kBlue:
        return packedColor & 0xFF;
      default:
        DriverStation.reportWarning("Unknown color channel for unpacking: " + channel, true);
        return 0;
    }
  }

  /**
   * Performs a linear interpolation between two colors in the RGB colorspace.
   *
   * @param a the first color to interpolate from
   * @param b the second color to interpolate from
   * @param t the interpolation scale in [0, 1]
   * @return the interpolated color
   */
  public static Color lerpRGB(Color a, Color b, double t) {
    int packedRGB = lerpRGB(a.red, a.green, a.blue, b.red, b.green, b.blue, t);

    return new Color(
        unpackRGB(packedRGB, RGBChannel.kRed),
        unpackRGB(packedRGB, RGBChannel.kGreen),
        unpackRGB(packedRGB, RGBChannel.kBlue));
  }

  /**
   * Linearly interpolates between two RGB colors represented by the (r1, g1, b1) and (r2, g2, b2)
   * triplets. For memory performance reasons, the output color is returned packed into a single
   * 32-bit integer; use {@link #unpackRGB(int, RGBChannel)} to extract the values for the
   * individual red, green, and blue channels.
   *
   * @param r1 the red value of the first color, in [0, 1]
   * @param g1 the green value of the first color, in [0, 1]
   * @param b1 the blue value of the first color, in [0, 1]
   * @param r2 the red value of the second color, in [0, 1]
   * @param g2 the green value of the second color, in [0, 1]
   * @param b2 the blue value of the second color, in [0, 1]
   * @param t the interpolation value, in [0, 1]
   * @return the interpolated color, packed in a 32-bit integer
   */
  public static int lerpRGB(
      double r1, double g1, double b1, double r2, double g2, double b2, double t) {
    return lerpRGB(
        (int) (r1 * 255),
        (int) (g1 * 255),
        (int) (b1 * 255),
        (int) (r2 * 255),
        (int) (g2 * 255),
        (int) (b2 * 255),
        t);
  }

  /**
   * Linearly interpolates between two RGB colors represented by the (r1, g1, b1) and (r2, g2, b2)
   * triplets. For memory performance reasons, the output color is returned packed into a single
   * 32-bit integer; use {@link #unpackRGB(int, RGBChannel)} to extract the values for the
   * individual red, green, and blue channels.
   *
   * @param r1 the red value of the first color, in [0, 255]
   * @param g1 the green value of the first color, in [0, 255]
   * @param b1 the blue value of the first color, in [0, 255]
   * @param r2 the red value of the second color, in [0, 255]
   * @param g2 the green value of the second color, in [0, 255]
   * @param b2 the blue value of the second color, in [0, 255]
   * @param t the interpolation value, in [0, 1]
   * @return the interpolated color, packed in a 32-bit integer
   */
  public static int lerpRGB(int r1, int g1, int b1, int r2, int g2, int b2, double t) {
    return packRGB(
        (int) MathUtil.interpolate(r1, r2, t),
        (int) MathUtil.interpolate(g1, g2, t),
        (int) MathUtil.interpolate(b1, b2, t));
  }

  /*
   * FIRST Colors
   */

  /** 0x1560BD. */
  public static final Color kDenim = new Color(0.0823529412, 0.376470589, 0.7411764706, "kDenim");

  /** 0x0066B3. */
  public static final Color kFirstBlue = new Color(0.0, 0.4, 0.7019607844, "kFirstBlue");

  /** 0xED1C24. */
  public static final Color kFirstRed =
      new Color(0.9294117648, 0.1098039216, 0.1411764706, "kFirstRed");

  /*
   * Standard Colors
   */

  /** 0xF0F8FF. */
  public static final Color kAliceBlue = new Color(0.9411765f, 0.972549f, 1.0f, "kAliceBlue");

  /** 0xFAEBD7. */
  public static final Color kAntiqueWhite =
      new Color(0.98039216f, 0.92156863f, 0.84313726f, "kAntiqueWhite");

  /** 0x00FFFF. */
  public static final Color kAqua = new Color(0.0f, 1.0f, 1.0f, "kAqua");

  /** 0x7FFFD4. */
  public static final Color kAquamarine = new Color(0.49803922f, 1.0f, 0.83137256f, "kAquamarine");

  /** 0xF0FFFF. */
  public static final Color kAzure = new Color(0.9411765f, 1.0f, 1.0f, "kAzure");

  /** 0xF5F5DC. */
  public static final Color kBeige = new Color(0.9607843f, 0.9607843f, 0.8627451f, "kBeige");

  /** 0xFFE4C4. */
  public static final Color kBisque = new Color(1.0f, 0.89411765f, 0.76862746f, "kBisque");

  /** 0x000000. */
  public static final Color kBlack = new Color(0.0f, 0.0f, 0.0f, "kBlack");

  /** 0xFFEBCD. */
  public static final Color kBlanchedAlmond =
      new Color(1.0f, 0.92156863f, 0.8039216f, "kBlanchedAlmond");

  /** 0x0000FF. */
  public static final Color kBlue = new Color(0.0f, 0.0f, 1.0f, "kBlue");

  /** 0x8A2BE2. */
  public static final Color kBlueViolet =
      new Color(0.5411765f, 0.16862746f, 0.8862745f, "kBlueViolet");

  /** 0xA52A2A. */
  public static final Color kBrown = new Color(0.64705884f, 0.16470589f, 0.16470589f, "kBrown");

  /** 0xDEB887. */
  public static final Color kBurlywood =
      new Color(0.87058824f, 0.72156864f, 0.5294118f, "kBurlywood");

  /** 0x5F9EA0. */
  public static final Color kCadetBlue =
      new Color(0.37254903f, 0.61960787f, 0.627451f, "kCadetBlue");

  /** 0x7FFF00. */
  public static final Color kChartreuse = new Color(0.49803922f, 1.0f, 0.0f, "kChartreuse");

  /** 0xD2691E. */
  public static final Color kChocolate =
      new Color(0.8235294f, 0.4117647f, 0.11764706f, "kChocolate");

  /** 0xFF7F50. */
  public static final Color kCoral = new Color(1.0f, 0.49803922f, 0.3137255f, "kCoral");

  /** 0x6495ED. */
  public static final Color kCornflowerBlue =
      new Color(0.39215687f, 0.58431375f, 0.92941177f, "kCornflowerBlue");

  /** 0xFFF8DC. */
  public static final Color kCornsilk = new Color(1.0f, 0.972549f, 0.8627451f, "kCornsilk");

  /** 0xDC143C. */
  public static final Color kCrimson = new Color(0.8627451f, 0.078431375f, 0.23529412f, "kCrimson");

  /** 0x00FFFF. */
  public static final Color kCyan = new Color(0.0f, 1.0f, 1.0f, "kCyan");

  /** 0x00008B. */
  public static final Color kDarkBlue = new Color(0.0f, 0.0f, 0.54509807f, "kDarkBlue");

  /** 0x008B8B. */
  public static final Color kDarkCyan = new Color(0.0f, 0.54509807f, 0.54509807f, "kDarkCyan");

  /** 0xB8860B. */
  public static final Color kDarkGoldenrod =
      new Color(0.72156864f, 0.5254902f, 0.043137256f, "kDarkGoldenrod");

  /** 0xA9A9A9. */
  public static final Color kDarkGray = new Color(0.6627451f, 0.6627451f, 0.6627451f, "kDarkGray");

  /** 0x006400. */
  public static final Color kDarkGreen = new Color(0.0f, 0.39215687f, 0.0f, "kDarkGreen");

  /** 0xBDB76B. */
  public static final Color kDarkKhaki =
      new Color(0.7411765f, 0.7176471f, 0.41960785f, "kDarkKhaki");

  /** 0x8B008B. */
  public static final Color kDarkMagenta =
      new Color(0.54509807f, 0.0f, 0.54509807f, "kDarkMagenta");

  /** 0x556B2F. */
  public static final Color kDarkOliveGreen =
      new Color(0.33333334f, 0.41960785f, 0.18431373f, "kDarkOliveGreen");

  /** 0xFF8C00. */
  public static final Color kDarkOrange = new Color(1.0f, 0.54901963f, 0.0f, "kDarkOrange");

  /** 0x9932CC. */
  public static final Color kDarkOrchid = new Color(0.6f, 0.19607843f, 0.8f, "kDarkOrchid");

  /** 0x8B0000. */
  public static final Color kDarkRed = new Color(0.54509807f, 0.0f, 0.0f, "kDarkRed");

  /** 0xE9967A. */
  public static final Color kDarkSalmon =
      new Color(0.9137255f, 0.5882353f, 0.47843137f, "kDarkSalmon");

  /** 0x8FBC8F. */
  public static final Color kDarkSeaGreen =
      new Color(0.56078434f, 0.7372549f, 0.56078434f, "kDarkSeaGreen");

  /** 0x483D8B. */
  public static final Color kDarkSlateBlue =
      new Color(0.28235295f, 0.23921569f, 0.54509807f, "kDarkSlateBlue");

  /** 0x2F4F4F. */
  public static final Color kDarkSlateGray =
      new Color(0.18431373f, 0.30980393f, 0.30980393f, "kDarkSlateGray");

  /** 0x00CED1. */
  public static final Color kDarkTurquoise =
      new Color(0.0f, 0.80784315f, 0.81960785f, "kDarkTurquoise");

  /** 0x9400D3. */
  public static final Color kDarkViolet = new Color(0.5803922f, 0.0f, 0.827451f, "kDarkViolet");

  /** 0xFF1493. */
  public static final Color kDeepPink = new Color(1.0f, 0.078431375f, 0.5764706f, "kDeepPink");

  /** 0x00BFFF. */
  public static final Color kDeepSkyBlue = new Color(0.0f, 0.7490196f, 1.0f, "kDeepSkyBlue");

  /** 0x696969. */
  public static final Color kDimGray = new Color(0.4117647f, 0.4117647f, 0.4117647f, "kDimGray");

  /** 0x1E90FF. */
  public static final Color kDodgerBlue = new Color(0.11764706f, 0.5647059f, 1.0f, "kDodgerBlue");

  /** 0xB22222. */
  public static final Color kFirebrick =
      new Color(0.69803923f, 0.13333334f, 0.13333334f, "kFirebrick");

  /** 0xFFFAF0. */
  public static final Color kFloralWhite = new Color(1.0f, 0.98039216f, 0.9411765f, "kFloralWhite");

  /** 0x228B22. */
  public static final Color kForestGreen =
      new Color(0.13333334f, 0.54509807f, 0.13333334f, "kForestGreen");

  /** 0xFF00FF. */
  public static final Color kFuchsia = new Color(1.0f, 0.0f, 1.0f, "kFuchsia");

  /** 0xDCDCDC. */
  public static final Color kGainsboro =
      new Color(0.8627451f, 0.8627451f, 0.8627451f, "kGainsboro");

  /** 0xF8F8FF. */
  public static final Color kGhostWhite = new Color(0.972549f, 0.972549f, 1.0f, "kGhostWhite");

  /** 0xFFD700. */
  public static final Color kGold = new Color(1.0f, 0.84313726f, 0.0f, "kGold");

  /** 0xDAA520. */
  public static final Color kGoldenrod =
      new Color(0.85490197f, 0.64705884f, 0.1254902f, "kGoldenrod");

  /** 0x808080. */
  public static final Color kGray = new Color(0.5019608f, 0.5019608f, 0.5019608f, "kGray");

  /** 0x008000. */
  public static final Color kGreen = new Color(0.0f, 0.5019608f, 0.0f, "kGreen");

  /** 0xADFF2F. */
  public static final Color kGreenYellow = new Color(0.6784314f, 1.0f, 0.18431373f, "kGreenYellow");

  /** 0xF0FFF0. */
  public static final Color kHoneydew = new Color(0.9411765f, 1.0f, 0.9411765f, "kHoneydew");

  /** 0xFF69B4. */
  public static final Color kHotPink = new Color(1.0f, 0.4117647f, 0.7058824f, "kHotPink");

  /** 0xCD5C5C. */
  public static final Color kIndianRed =
      new Color(0.8039216f, 0.36078432f, 0.36078432f, "kIndianRed");

  /** 0x4B0082. */
  public static final Color kIndigo = new Color(0.29411766f, 0.0f, 0.50980395f, "kIndigo");

  /** 0xFFFFF0. */
  public static final Color kIvory = new Color(1.0f, 1.0f, 0.9411765f, "kIvory");

  /** 0xF0E68C. */
  public static final Color kKhaki = new Color(0.9411765f, 0.9019608f, 0.54901963f, "kKhaki");

  /** 0xE6E6FA. */
  public static final Color kLavender = new Color(0.9019608f, 0.9019608f, 0.98039216f, "kLavender");

  /** 0xFFF0F5. */
  public static final Color kLavenderBlush =
      new Color(1.0f, 0.9411765f, 0.9607843f, "kLavenderBlush");

  /** 0x7CFC00. */
  public static final Color kLawnGreen = new Color(0.4862745f, 0.9882353f, 0.0f, "kLawnGreen");

  /** 0xFFFACD. */
  public static final Color kLemonChiffon =
      new Color(1.0f, 0.98039216f, 0.8039216f, "kLemonChiffon");

  /** 0xADD8E6. */
  public static final Color kLightBlue =
      new Color(0.6784314f, 0.84705883f, 0.9019608f, "kLightBlue");

  /** 0xF08080. */
  public static final Color kLightCoral =
      new Color(0.9411765f, 0.5019608f, 0.5019608f, "kLightCoral");

  /** 0xE0FFFF. */
  public static final Color kLightCyan = new Color(0.8784314f, 1.0f, 1.0f, "kLightCyan");

  /** 0xFAFAD2. */
  public static final Color kLightGoldenrodYellow =
      new Color(0.98039216f, 0.98039216f, 0.8235294f, "kLightGoldenrodYellow");

  /** 0xD3D3D3. */
  public static final Color kLightGray = new Color(0.827451f, 0.827451f, 0.827451f, "kLightGray");

  /** 0x90EE90. */
  public static final Color kLightGreen =
      new Color(0.5647059f, 0.93333334f, 0.5647059f, "kLightGreen");

  /** 0xFFB6C1. */
  public static final Color kLightPink = new Color(1.0f, 0.7137255f, 0.75686276f, "kLightPink");

  /** 0xFFA07A. */
  public static final Color kLightSalmon = new Color(1.0f, 0.627451f, 0.47843137f, "kLightSalmon");

  /** 0x20B2AA. */
  public static final Color kLightSeaGreen =
      new Color(0.1254902f, 0.69803923f, 0.6666667f, "kLightSeaGreen");

  /** 0x87CEFA. */
  public static final Color kLightSkyBlue =
      new Color(0.5294118f, 0.80784315f, 0.98039216f, "kLightSkyBlue");

  /** 0x778899. */
  public static final Color kLightSlateGray =
      new Color(0.46666667f, 0.53333336f, 0.6f, "kLightSlateGray");

  /** 0xB0C4DE. */
  public static final Color kLightSteelBlue =
      new Color(0.6901961f, 0.76862746f, 0.87058824f, "kLightSteelBlue");

  /** 0xFFFFE0. */
  public static final Color kLightYellow = new Color(1.0f, 1.0f, 0.8784314f, "kLightYellow");

  /** 0x00FF00. */
  public static final Color kLime = new Color(0.0f, 1.0f, 0.0f, "kLime");

  /** 0x32CD32. */
  public static final Color kLimeGreen =
      new Color(0.19607843f, 0.8039216f, 0.19607843f, "kLimeGreen");

  /** 0xFAF0E6. */
  public static final Color kLinen = new Color(0.98039216f, 0.9411765f, 0.9019608f, "kLinen");

  /** 0xFF00FF. */
  public static final Color kMagenta = new Color(1.0f, 0.0f, 1.0f, "kMagenta");

  /** 0x800000. */
  public static final Color kMaroon = new Color(0.5019608f, 0.0f, 0.0f, "kMaroon");

  /** 0x66CDAA. */
  public static final Color kMediumAquamarine =
      new Color(0.4f, 0.8039216f, 0.6666667f, "kMediumAquamarine");

  /** 0x0000CD. */
  public static final Color kMediumBlue = new Color(0.0f, 0.0f, 0.8039216f, "kMediumBlue");

  /** 0xBA55D3. */
  public static final Color kMediumOrchid =
      new Color(0.7294118f, 0.33333334f, 0.827451f, "kMediumOrchid");

  /** 0x9370DB. */
  public static final Color kMediumPurple =
      new Color(0.5764706f, 0.4392157f, 0.85882354f, "kMediumPurple");

  /** 0x3CB371. */
  public static final Color kMediumSeaGreen =
      new Color(0.23529412f, 0.7019608f, 0.44313726f, "kMediumSeaGreen");

  /** 0x7B68EE. */
  public static final Color kMediumSlateBlue =
      new Color(0.48235294f, 0.40784314f, 0.93333334f, "kMediumSlateBlue");

  /** 0x00FA9A. */
  public static final Color kMediumSpringGreen =
      new Color(0.0f, 0.98039216f, 0.6039216f, "kMediumSpringGreen");

  /** 0x48D1CC. */
  public static final Color kMediumTurquoise =
      new Color(0.28235295f, 0.81960785f, 0.8f, "kMediumTurquoise");

  /** 0xC71585. */
  public static final Color kMediumVioletRed =
      new Color(0.78039217f, 0.08235294f, 0.52156866f, "kMediumVioletRed");

  /** 0x191970. */
  public static final Color kMidnightBlue =
      new Color(0.09803922f, 0.09803922f, 0.4392157f, "kMidnightBlue");

  /** 0xF5FFFA. */
  public static final Color kMintcream = new Color(0.9607843f, 1.0f, 0.98039216f, "kMintcream");

  /** 0xFFE4E1. */
  public static final Color kMistyRose = new Color(1.0f, 0.89411765f, 0.88235295f, "kMistyRose");

  /** 0xFFE4B5. */
  public static final Color kMoccasin = new Color(1.0f, 0.89411765f, 0.70980394f, "kMoccasin");

  /** 0xFFDEAD. */
  public static final Color kNavajoWhite = new Color(1.0f, 0.87058824f, 0.6784314f, "kNavajoWhite");

  /** 0x000080. */
  public static final Color kNavy = new Color(0.0f, 0.0f, 0.5019608f, "kNavy");

  /** 0xFDF5E6. */
  public static final Color kOldLace = new Color(0.99215686f, 0.9607843f, 0.9019608f, "kOldLace");

  /** 0x808000. */
  public static final Color kOlive = new Color(0.5019608f, 0.5019608f, 0.0f, "kOlive");

  /** 0x6B8E23. */
  public static final Color kOliveDrab =
      new Color(0.41960785f, 0.5568628f, 0.13725491f, "kOliveDrab");

  /** 0xFFA500. */
  public static final Color kOrange = new Color(1.0f, 0.64705884f, 0.0f, "kOrange");

  /** 0xFF4500. */
  public static final Color kOrangeRed = new Color(1.0f, 0.27058825f, 0.0f, "kOrangeRed");

  /** 0xDA70D6. */
  public static final Color kOrchid = new Color(0.85490197f, 0.4392157f, 0.8392157f, "kOrchid");

  /** 0xEEE8AA. */
  public static final Color kPaleGoldenrod =
      new Color(0.93333334f, 0.9098039f, 0.6666667f, "kPaleGoldenrod");

  /** 0x98FB98. */
  public static final Color kPaleGreen =
      new Color(0.59607846f, 0.9843137f, 0.59607846f, "kPaleGreen");

  /** 0xAFEEEE. */
  public static final Color kPaleTurquoise =
      new Color(0.6862745f, 0.93333334f, 0.93333334f, "kPaleTurquoise");

  /** 0xDB7093. */
  public static final Color kPaleVioletRed =
      new Color(0.85882354f, 0.4392157f, 0.5764706f, "kPaleVioletRed");

  /** 0xFFEFD5. */
  public static final Color kPapayaWhip = new Color(1.0f, 0.9372549f, 0.8352941f, "kPapayaWhip");

  /** 0xFFDAB9. */
  public static final Color kPeachPuff = new Color(1.0f, 0.85490197f, 0.7254902f, "kPeachPuff");

  /** 0xCD853F. */
  public static final Color kPeru = new Color(0.8039216f, 0.52156866f, 0.24705882f, "kPeru");

  /** 0xFFC0CB. */
  public static final Color kPink = new Color(1.0f, 0.7529412f, 0.79607844f, "kPink");

  /** 0xDDA0DD. */
  public static final Color kPlum = new Color(0.8666667f, 0.627451f, 0.8666667f, "kPlum");

  /** 0xB0E0E6. */
  public static final Color kPowderBlue =
      new Color(0.6901961f, 0.8784314f, 0.9019608f, "kPowderBlue");

  /** 0x800080. */
  public static final Color kPurple = new Color(0.5019608f, 0.0f, 0.5019608f, "kPurple");

  /** 0xFF0000. */
  public static final Color kRed = new Color(1.0f, 0.0f, 0.0f, "kRed");

  /** 0xBC8F8F. */
  public static final Color kRosyBrown =
      new Color(0.7372549f, 0.56078434f, 0.56078434f, "kRosyBrown");

  /** 0x4169E1. */
  public static final Color kRoyalBlue =
      new Color(0.25490198f, 0.4117647f, 0.88235295f, "kRoyalBlue");

  /** 0x8B4513. */
  public static final Color kSaddleBrown =
      new Color(0.54509807f, 0.27058825f, 0.07450981f, "kSaddleBrown");

  /** 0xFA8072. */
  public static final Color kSalmon = new Color(0.98039216f, 0.5019608f, 0.44705883f, "kSalmon");

  /** 0xF4A460. */
  public static final Color kSandyBrown =
      new Color(0.95686275f, 0.6431373f, 0.3764706f, "kSandyBrown");

  /** 0x2E8B57. */
  public static final Color kSeaGreen =
      new Color(0.18039216f, 0.54509807f, 0.34117648f, "kSeaGreen");

  /** 0xFFF5EE. */
  public static final Color kSeashell = new Color(1.0f, 0.9607843f, 0.93333334f, "kSeashell");

  /** 0xA0522D. */
  public static final Color kSienna = new Color(0.627451f, 0.32156864f, 0.1764706f, "kSienna");

  /** 0xC0C0C0. */
  public static final Color kSilver = new Color(0.7529412f, 0.7529412f, 0.7529412f, "kSilver");

  /** 0x87CEEB. */
  public static final Color kSkyBlue = new Color(0.5294118f, 0.80784315f, 0.92156863f, "kSkyBlue");

  /** 0x6A5ACD. */
  public static final Color kSlateBlue =
      new Color(0.41568628f, 0.3529412f, 0.8039216f, "kSlateBlue");

  /** 0x708090. */
  public static final Color kSlateGray =
      new Color(0.4392157f, 0.5019608f, 0.5647059f, "kSlateGray");

  /** 0xFFFAFA. */
  public static final Color kSnow = new Color(1.0f, 0.98039216f, 0.98039216f, "kSnow");

  /** 0x00FF7F. */
  public static final Color kSpringGreen = new Color(0.0f, 1.0f, 0.49803922f, "kSpringGreen");

  /** 0x4682B4. */
  public static final Color kSteelBlue =
      new Color(0.27450982f, 0.50980395f, 0.7058824f, "kSteelBlue");

  /** 0xD2B48C. */
  public static final Color kTan = new Color(0.8235294f, 0.7058824f, 0.54901963f, "kTan");

  /** 0x008080. */
  public static final Color kTeal = new Color(0.0f, 0.5019608f, 0.5019608f, "kTeal");

  /** 0xD8BFD8. */
  public static final Color kThistle = new Color(0.84705883f, 0.7490196f, 0.84705883f, "kThistle");

  /** 0xFF6347. */
  public static final Color kTomato = new Color(1.0f, 0.3882353f, 0.2784314f, "kTomato");

  /** 0x40E0D0. */
  public static final Color kTurquoise =
      new Color(0.2509804f, 0.8784314f, 0.8156863f, "kTurquoise");

  /** 0xEE82EE. */
  public static final Color kViolet = new Color(0.93333334f, 0.50980395f, 0.93333334f, "kViolet");

  /** 0xF5DEB3. */
  public static final Color kWheat = new Color(0.9607843f, 0.87058824f, 0.7019608f, "kWheat");

  /** 0xFFFFFF. */
  public static final Color kWhite = new Color(1.0f, 1.0f, 1.0f, "kWhite");

  /** 0xF5F5F5. */
  public static final Color kWhiteSmoke =
      new Color(0.9607843f, 0.9607843f, 0.9607843f, "kWhiteSmoke");

  /** 0xFFFF00. */
  public static final Color kYellow = new Color(1.0f, 1.0f, 0.0f, "kYellow");

  /** 0x9ACD32. */
  public static final Color kYellowGreen =
      new Color(0.6039216f, 0.8039216f, 0.19607843f, "kYellowGreen");
}
