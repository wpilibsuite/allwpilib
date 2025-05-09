// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.util;

import edu.wpi.first.math.MathUtil;
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
        unpackRGB(rgb, RGBChannel.RED),
        unpackRGB(rgb, RGBChannel.GREEN),
        unpackRGB(rgb, RGBChannel.BLUE));
  }

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null) {
      return false;
    }

    return other instanceof Color color
        && Double.compare(color.red, red) == 0
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
    RED,
    /** The green channel of an RGB color. */
    GREEN,
    /** The blue channel of an RGB color. */
    BLUE
  }

  /**
   * Packs 3 RGB values into a single 32-bit integer. These values can be unpacked with {@link
   * #unpackRGB(int, RGBChannel)} to retrieve the values. This is helpful for avoiding memory
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
    return switch (channel) {
      case RED -> (packedColor >> 16) & 0xFF;
      case GREEN -> (packedColor >> 8) & 0xFF;
      case BLUE -> packedColor & 0xFF;
    };
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
        unpackRGB(packedRGB, RGBChannel.RED),
        unpackRGB(packedRGB, RGBChannel.GREEN),
        unpackRGB(packedRGB, RGBChannel.BLUE));
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
  public static final Color DENIM = new Color(0.0823529412, 0.376470589, 0.7411764706, "DENIM");

  /** 0x0066B3. */
  public static final Color FIRST_BLUE = new Color(0.0, 0.4, 0.7019607844, "FIRST_BLUE");

  /** 0xED1C24. */
  public static final Color FIRST_RED =
      new Color(0.9294117648, 0.1098039216, 0.1411764706, "FIRST_RED");

  /*
   * Standard Colors
   */

  /** 0xF0F8FF. */
  public static final Color ALICE_BLUE = new Color(0.9411765f, 0.972549f, 1.0f, "ALICE_BLUE");

  /** 0xFAEBD7. */
  public static final Color ANTIQUE_WHITE =
      new Color(0.98039216f, 0.92156863f, 0.84313726f, "ANTIQUE_WHITE");

  /** 0x00FFFF. */
  public static final Color AGUA = new Color(0.0f, 1.0f, 1.0f, "AGUA");

  /** 0x7FFFD4. */
  public static final Color AQUAMARINE = new Color(0.49803922f, 1.0f, 0.83137256f, "AQUAMARINE");

  /** 0xF0FFFF. */
  public static final Color AZURE = new Color(0.9411765f, 1.0f, 1.0f, "AZURE");

  /** 0xF5F5DC. */
  public static final Color BEIGE = new Color(0.9607843f, 0.9607843f, 0.8627451f, "BEIGE");

  /** 0xFFE4C4. */
  public static final Color BISQUE = new Color(1.0f, 0.89411765f, 0.76862746f, "BISQUE");

  /** 0x000000. */
  public static final Color BLACK = new Color(0.0f, 0.0f, 0.0f, "BLACK");

  /** 0xFFEBCD. */
  public static final Color BLANCHED_ALMOND =
      new Color(1.0f, 0.92156863f, 0.8039216f, "BLANCHED_ALMOND");

  /** 0x0000FF. */
  public static final Color BLUE = new Color(0.0f, 0.0f, 1.0f, "BLUE");

  /** 0x8A2BE2. */
  public static final Color BLUE_VIOLET =
      new Color(0.5411765f, 0.16862746f, 0.8862745f, "BLUE_VIOLET");

  /** 0xA52A2A. */
  public static final Color BROWN = new Color(0.64705884f, 0.16470589f, 0.16470589f, "BROWN");

  /** 0xDEB887. */
  public static final Color BURLYWOOD =
      new Color(0.87058824f, 0.72156864f, 0.5294118f, "BURLYWOOD");

  /** 0x5F9EA0. */
  public static final Color CADET_BLUE =
      new Color(0.37254903f, 0.61960787f, 0.627451f, "CADET_BLUE");

  /** 0x7FFF00. */
  public static final Color CHARTREUSE = new Color(0.49803922f, 1.0f, 0.0f, "CHARTREUSE");

  /** 0xD2691E. */
  public static final Color CHOCOLATE = new Color(0.8235294f, 0.4117647f, 0.11764706f, "CHOCOLATE");

  /** 0xFF7F50. */
  public static final Color CORAL = new Color(1.0f, 0.49803922f, 0.3137255f, "CORAL");

  /** 0x6495ED. */
  public static final Color CORNFLOWER_BLUE =
      new Color(0.39215687f, 0.58431375f, 0.92941177f, "CORNFLOWER_BLUE");

  /** 0xFFF8DC. */
  public static final Color CORNSILK = new Color(1.0f, 0.972549f, 0.8627451f, "CORNSILK");

  /** 0xDC143C. */
  public static final Color CRIMSON = new Color(0.8627451f, 0.078431375f, 0.23529412f, "CRIMSON");

  /** 0x00FFFF. */
  public static final Color CYAN = new Color(0.0f, 1.0f, 1.0f, "CYAN");

  /** 0x00008B. */
  public static final Color DARK_BLUE = new Color(0.0f, 0.0f, 0.54509807f, "DARK_BLUE");

  /** 0x008B8B. */
  public static final Color DARK_CYAN = new Color(0.0f, 0.54509807f, 0.54509807f, "DARK_CYAN");

  /** 0xB8860B. */
  public static final Color DARK_GOLDENROD =
      new Color(0.72156864f, 0.5254902f, 0.043137256f, "DARK_GOLDENROD");

  /** 0xA9A9A9. */
  public static final Color DARK_GRAY = new Color(0.6627451f, 0.6627451f, 0.6627451f, "DARK_GRAY");

  /** 0x006400. */
  public static final Color DARK_GREEN = new Color(0.0f, 0.39215687f, 0.0f, "DARK_GREEN");

  /** 0xBDB76B. */
  public static final Color DARK_KHAKI =
      new Color(0.7411765f, 0.7176471f, 0.41960785f, "DARK_KHAKI");

  /** 0x8B008B. */
  public static final Color DARK_MAGENTA =
      new Color(0.54509807f, 0.0f, 0.54509807f, "DARK_MAGENTA");

  /** 0x556B2F. */
  public static final Color DARK_OLIVE_GREEN =
      new Color(0.33333334f, 0.41960785f, 0.18431373f, "DARK_OLIVE_GREEN");

  /** 0xFF8C00. */
  public static final Color DARK_ORANGE = new Color(1.0f, 0.54901963f, 0.0f, "DARK_ORANGE");

  /** 0x9932CC. */
  public static final Color DARK_ORCHID = new Color(0.6f, 0.19607843f, 0.8f, "DARK_ORCHID");

  /** 0x8B0000. */
  public static final Color DARK_RED = new Color(0.54509807f, 0.0f, 0.0f, "DARK_RED");

  /** 0xE9967A. */
  public static final Color DARK_SALMON =
      new Color(0.9137255f, 0.5882353f, 0.47843137f, "DARK_SALMON");

  /** 0x8FBC8F. */
  public static final Color DARK_SEA_GREEN =
      new Color(0.56078434f, 0.7372549f, 0.56078434f, "DARK_SEA_GREEN");

  /** 0x483D8B. */
  public static final Color DARK_SLATE_BLUE =
      new Color(0.28235295f, 0.23921569f, 0.54509807f, "DARK_SLATE_BLUE");

  /** 0x2F4F4F. */
  public static final Color DARK_SLATE_GRAY =
      new Color(0.18431373f, 0.30980393f, 0.30980393f, "DARK_SLATE_GRAY");

  /** 0x00CED1. */
  public static final Color DARK_TURQUOISE =
      new Color(0.0f, 0.80784315f, 0.81960785f, "DARK_TURQUOISE");

  /** 0x9400D3. */
  public static final Color DARK_VIOLET = new Color(0.5803922f, 0.0f, 0.827451f, "DARK_VIOLET");

  /** 0xFF1493. */
  public static final Color DEEP_PINK = new Color(1.0f, 0.078431375f, 0.5764706f, "DEEP_PINK");

  /** 0x00BFFF. */
  public static final Color DEEP_SKY_BLUE = new Color(0.0f, 0.7490196f, 1.0f, "DEEP_SKY_BLUE");

  /** 0x696969. */
  public static final Color DIM_GRAY = new Color(0.4117647f, 0.4117647f, 0.4117647f, "DIM_GRAY");

  /** 0x1E90FF. */
  public static final Color DODGER_BLUE = new Color(0.11764706f, 0.5647059f, 1.0f, "DODGER_BLUE");

  /** 0xB22222. */
  public static final Color FIREBRICK =
      new Color(0.69803923f, 0.13333334f, 0.13333334f, "FIREBRICK");

  /** 0xFFFAF0. */
  public static final Color FLORAL_WHITE = new Color(1.0f, 0.98039216f, 0.9411765f, "FLORAL_WHITE");

  /** 0x228B22. */
  public static final Color FOREST_GREEN =
      new Color(0.13333334f, 0.54509807f, 0.13333334f, "FOREST_GREEN");

  /** 0xFF00FF. */
  public static final Color FUCHSIA = new Color(1.0f, 0.0f, 1.0f, "FUCHSIA");

  /** 0xDCDCDC. */
  public static final Color GAINSBORO = new Color(0.8627451f, 0.8627451f, 0.8627451f, "GAINSBORO");

  /** 0xF8F8FF. */
  public static final Color GHOST_WHITE = new Color(0.972549f, 0.972549f, 1.0f, "GHOST_WHITE");

  /** 0xFFD700. */
  public static final Color GOLD = new Color(1.0f, 0.84313726f, 0.0f, "GOLD");

  /** 0xDAA520. */
  public static final Color GOLDENROD =
      new Color(0.85490197f, 0.64705884f, 0.1254902f, "GOLDENROD");

  /** 0x808080. */
  public static final Color GRAY = new Color(0.5019608f, 0.5019608f, 0.5019608f, "GRAY");

  /** 0x008000. */
  public static final Color GREEN = new Color(0.0f, 0.5019608f, 0.0f, "GREEN");

  /** 0xADFF2F. */
  public static final Color GREEN_YELLOW = new Color(0.6784314f, 1.0f, 0.18431373f, "GREEN_YELLOW");

  /** 0xF0FFF0. */
  public static final Color HONEYDEW = new Color(0.9411765f, 1.0f, 0.9411765f, "HONEYDEW");

  /** 0xFF69B4. */
  public static final Color HOT_PINK = new Color(1.0f, 0.4117647f, 0.7058824f, "HOT_PINK");

  /** 0xCD5C5C. */
  public static final Color INDIAN_RED =
      new Color(0.8039216f, 0.36078432f, 0.36078432f, "INDIAN_RED");

  /** 0x4B0082. */
  public static final Color INDIGO = new Color(0.29411766f, 0.0f, 0.50980395f, "INDIGO");

  /** 0xFFFFF0. */
  public static final Color IVORY = new Color(1.0f, 1.0f, 0.9411765f, "IVORY");

  /** 0xF0E68C. */
  public static final Color KHAKI = new Color(0.9411765f, 0.9019608f, 0.54901963f, "KHAKI");

  /** 0xE6E6FA. */
  public static final Color LAVENDER = new Color(0.9019608f, 0.9019608f, 0.98039216f, "LAVENDER");

  /** 0xFFF0F5. */
  public static final Color LAVENDER_BLUSH =
      new Color(1.0f, 0.9411765f, 0.9607843f, "LAVENDER_BLUSH");

  /** 0x7CFC00. */
  public static final Color LAWN_GREEN = new Color(0.4862745f, 0.9882353f, 0.0f, "LAWN_GREEN");

  /** 0xFFFACD. */
  public static final Color LEMON_CHIFFON =
      new Color(1.0f, 0.98039216f, 0.8039216f, "LEMON_CHIFFON");

  /** 0xADD8E6. */
  public static final Color LIGHT_BLUE =
      new Color(0.6784314f, 0.84705883f, 0.9019608f, "LIGHT_BLUE");

  /** 0xF08080. */
  public static final Color LIGHT_CORAL =
      new Color(0.9411765f, 0.5019608f, 0.5019608f, "LIGHT_CORAL");

  /** 0xE0FFFF. */
  public static final Color LIGHT_CYAN = new Color(0.8784314f, 1.0f, 1.0f, "LIGHT_CYAN");

  /** 0xFAFAD2. */
  public static final Color LIGHT_GOLDENROD_YELLOW =
      new Color(0.98039216f, 0.98039216f, 0.8235294f, "LIGHT_GOLDENROD_YELLOW");

  /** 0xD3D3D3. */
  public static final Color LIGHT_GRAY = new Color(0.827451f, 0.827451f, 0.827451f, "LIGHT_GRAY");

  /** 0x90EE90. */
  public static final Color LIGHT_GREEN =
      new Color(0.5647059f, 0.93333334f, 0.5647059f, "LIGHT_GREEN");

  /** 0xFFB6C1. */
  public static final Color LIGHT_PINK = new Color(1.0f, 0.7137255f, 0.75686276f, "LIGHT_PINK");

  /** 0xFFA07A. */
  public static final Color LIGHT_SALMON = new Color(1.0f, 0.627451f, 0.47843137f, "LIGHT_SALMON");

  /** 0x20B2AA. */
  public static final Color LIGHT_SEA_GREEN =
      new Color(0.1254902f, 0.69803923f, 0.6666667f, "LIGHT_SEA_GREEN");

  /** 0x87CEFA. */
  public static final Color LIGHT_SKY_BLUE =
      new Color(0.5294118f, 0.80784315f, 0.98039216f, "LIGHT_SKY_BLUE");

  /** 0x778899. */
  public static final Color LIGHT_SLATE_GRAY =
      new Color(0.46666667f, 0.53333336f, 0.6f, "LIGHT_SLATE_GRAY");

  /** 0xB0C4DE. */
  public static final Color LIGHT_STEEL_BLUE =
      new Color(0.6901961f, 0.76862746f, 0.87058824f, "LIGHT_STEEL_BLUE");

  /** 0xFFFFE0. */
  public static final Color LIGHT_YELLOW = new Color(1.0f, 1.0f, 0.8784314f, "LIGHT_YELLOW");

  /** 0x00FF00. */
  public static final Color LIME = new Color(0.0f, 1.0f, 0.0f, "LIME");

  /** 0x32CD32. */
  public static final Color LIME_GREEN =
      new Color(0.19607843f, 0.8039216f, 0.19607843f, "LIME_GREEN");

  /** 0xFAF0E6. */
  public static final Color LINEN = new Color(0.98039216f, 0.9411765f, 0.9019608f, "LINEN");

  /** 0xFF00FF. */
  public static final Color MAGENTA = new Color(1.0f, 0.0f, 1.0f, "MAGENTA");

  /** 0x800000. */
  public static final Color MAROON = new Color(0.5019608f, 0.0f, 0.0f, "MAROON");

  /** 0x66CDAA. */
  public static final Color MEDIUM_AQUAMARINE =
      new Color(0.4f, 0.8039216f, 0.6666667f, "MEDIUM_AQUAMARINE");

  /** 0x0000CD. */
  public static final Color MEDIUM_BLUE = new Color(0.0f, 0.0f, 0.8039216f, "MEDIUM_BLUE");

  /** 0xBA55D3. */
  public static final Color MEDIUM_ORCHID =
      new Color(0.7294118f, 0.33333334f, 0.827451f, "MEDIUM_ORCHID");

  /** 0x9370DB. */
  public static final Color MEDIUM_PURPLE =
      new Color(0.5764706f, 0.4392157f, 0.85882354f, "MEDIUM_PURPLE");

  /** 0x3CB371. */
  public static final Color MEDIUM_SEA_GREEN =
      new Color(0.23529412f, 0.7019608f, 0.44313726f, "MEDIUM_SEA_GREEN");

  /** 0x7B68EE. */
  public static final Color MEDIUM_SLATE_BLUE =
      new Color(0.48235294f, 0.40784314f, 0.93333334f, "MEDIUM_SLATE_BLUE");

  /** 0x00FA9A. */
  public static final Color MEDIUM_SPRING_GREEN =
      new Color(0.0f, 0.98039216f, 0.6039216f, "MEDIUM_SPRING_GREEN");

  /** 0x48D1CC. */
  public static final Color MEDIUM_TURQUOISE =
      new Color(0.28235295f, 0.81960785f, 0.8f, "MEDIUM_TURQUOISE");

  /** 0xC71585. */
  public static final Color MEDIUM_VIOLET_RED =
      new Color(0.78039217f, 0.08235294f, 0.52156866f, "MEDIUM_VIOLET_RED");

  /** 0x191970. */
  public static final Color MIDNIGHT_BLUE =
      new Color(0.09803922f, 0.09803922f, 0.4392157f, "MIDNIGHT_BLUE");

  /** 0xF5FFFA. */
  public static final Color MINTCREAM = new Color(0.9607843f, 1.0f, 0.98039216f, "MINTCREAM");

  /** 0xFFE4E1. */
  public static final Color MISTY_ROSE = new Color(1.0f, 0.89411765f, 0.88235295f, "MISTY_ROSE");

  /** 0xFFE4B5. */
  public static final Color MOCCASIN = new Color(1.0f, 0.89411765f, 0.70980394f, "MOCCASIN");

  /** 0xFFDEAD. */
  public static final Color NAVAJO_WHITE = new Color(1.0f, 0.87058824f, 0.6784314f, "NAVAJO_WHITE");

  /** 0x000080. */
  public static final Color NAVY = new Color(0.0f, 0.0f, 0.5019608f, "NAVY");

  /** 0xFDF5E6. */
  public static final Color OLD_LACE = new Color(0.99215686f, 0.9607843f, 0.9019608f, "OLD_LACE");

  /** 0x808000. */
  public static final Color OLIVE = new Color(0.5019608f, 0.5019608f, 0.0f, "OLIVE");

  /** 0x6B8E23. */
  public static final Color OLIVE_DRAB =
      new Color(0.41960785f, 0.5568628f, 0.13725491f, "OLIVE_DRAB");

  /** 0xFFA500. */
  public static final Color ORANGE = new Color(1.0f, 0.64705884f, 0.0f, "ORANGE");

  /** 0xFF4500. */
  public static final Color ORANGE_RED = new Color(1.0f, 0.27058825f, 0.0f, "ORANGE_RED");

  /** 0xDA70D6. */
  public static final Color ORCHID = new Color(0.85490197f, 0.4392157f, 0.8392157f, "ORCHID");

  /** 0xEEE8AA. */
  public static final Color PALE_GOLDENROD =
      new Color(0.93333334f, 0.9098039f, 0.6666667f, "PALE_GOLDENROD");

  /** 0x98FB98. */
  public static final Color PALE_GREEN =
      new Color(0.59607846f, 0.9843137f, 0.59607846f, "PALE_GREEN");

  /** 0xAFEEEE. */
  public static final Color PALE_TURQUOISE =
      new Color(0.6862745f, 0.93333334f, 0.93333334f, "PALE_TURQUOISE");

  /** 0xDB7093. */
  public static final Color PALE_VIOLET_RED =
      new Color(0.85882354f, 0.4392157f, 0.5764706f, "PALE_VIOLET_RED");

  /** 0xFFEFD5. */
  public static final Color PAPAYA_WHIP = new Color(1.0f, 0.9372549f, 0.8352941f, "PAPAYA_WHIP");

  /** 0xFFDAB9. */
  public static final Color PEACH_PUFF = new Color(1.0f, 0.85490197f, 0.7254902f, "PEACH_PUFF");

  /** 0xCD853F. */
  public static final Color PERU = new Color(0.8039216f, 0.52156866f, 0.24705882f, "PERU");

  /** 0xFFC0CB. */
  public static final Color PINK = new Color(1.0f, 0.7529412f, 0.79607844f, "PINK");

  /** 0xDDA0DD. */
  public static final Color PLUM = new Color(0.8666667f, 0.627451f, 0.8666667f, "PLUM");

  /** 0xB0E0E6. */
  public static final Color POWDER_BLUE =
      new Color(0.6901961f, 0.8784314f, 0.9019608f, "POWDER_BLUE");

  /** 0x800080. */
  public static final Color PURPLE = new Color(0.5019608f, 0.0f, 0.5019608f, "PURPLE");

  /** 0xFF0000. */
  public static final Color RED = new Color(1.0f, 0.0f, 0.0f, "RED");

  /** 0xBC8F8F. */
  public static final Color ROSY_BROWN =
      new Color(0.7372549f, 0.56078434f, 0.56078434f, "ROSY_BROWN");

  /** 0x4169E1. */
  public static final Color ROYAL_BLUE =
      new Color(0.25490198f, 0.4117647f, 0.88235295f, "ROYAL_BLUE");

  /** 0x8B4513. */
  public static final Color SADDLE_BROWN =
      new Color(0.54509807f, 0.27058825f, 0.07450981f, "SADDLE_BROWN");

  /** 0xFA8072. */
  public static final Color SALMON = new Color(0.98039216f, 0.5019608f, 0.44705883f, "SALMON");

  /** 0xF4A460. */
  public static final Color SANDY_BROWN =
      new Color(0.95686275f, 0.6431373f, 0.3764706f, "SANDY_BROWN");

  /** 0x2E8B57. */
  public static final Color SEA_GREEN =
      new Color(0.18039216f, 0.54509807f, 0.34117648f, "SEA_GREEN");

  /** 0xFFF5EE. */
  public static final Color SEASHELL = new Color(1.0f, 0.9607843f, 0.93333334f, "SEASHELL");

  /** 0xA0522D. */
  public static final Color SIENNA = new Color(0.627451f, 0.32156864f, 0.1764706f, "SIENNA");

  /** 0xC0C0C0. */
  public static final Color SILVER = new Color(0.7529412f, 0.7529412f, 0.7529412f, "SILVER");

  /** 0x87CEEB. */
  public static final Color SKY_BLUE = new Color(0.5294118f, 0.80784315f, 0.92156863f, "SKY_BLUE");

  /** 0x6A5ACD. */
  public static final Color SLATE_BLUE =
      new Color(0.41568628f, 0.3529412f, 0.8039216f, "SLATE_BLUE");

  /** 0x708090. */
  public static final Color SLATE_GRAY =
      new Color(0.4392157f, 0.5019608f, 0.5647059f, "SLATE_GRAY");

  /** 0xFFFAFA. */
  public static final Color SNOW = new Color(1.0f, 0.98039216f, 0.98039216f, "SNOW");

  /** 0x00FF7F. */
  public static final Color SPRING_GREEN = new Color(0.0f, 1.0f, 0.49803922f, "SPRING_GREEN");

  /** 0x4682B4. */
  public static final Color STEEL_BLUE =
      new Color(0.27450982f, 0.50980395f, 0.7058824f, "STEEL_BLUE");

  /** 0xD2B48C. */
  public static final Color TAN = new Color(0.8235294f, 0.7058824f, 0.54901963f, "TAN");

  /** 0x008080. */
  public static final Color TEAL = new Color(0.0f, 0.5019608f, 0.5019608f, "TEAL");

  /** 0xD8BFD8. */
  public static final Color THISTLE = new Color(0.84705883f, 0.7490196f, 0.84705883f, "THISTLE");

  /** 0xFF6347. */
  public static final Color TOMATO = new Color(1.0f, 0.3882353f, 0.2784314f, "TOMATO");

  /** 0x40E0D0. */
  public static final Color TURQUOISE = new Color(0.2509804f, 0.8784314f, 0.8156863f, "TURQUOISE");

  /** 0xEE82EE. */
  public static final Color VIOLET = new Color(0.93333334f, 0.50980395f, 0.93333334f, "VIOLET");

  /** 0xF5DEB3. */
  public static final Color WHEAT = new Color(0.9607843f, 0.87058824f, 0.7019608f, "WHEAT");

  /** 0xFFFFFF. */
  public static final Color WHITE = new Color(1.0f, 1.0f, 1.0f, "WHITE");

  /** 0xF5F5F5. */
  public static final Color WHITE_SMOKE =
      new Color(0.9607843f, 0.9607843f, 0.9607843f, "WHITE_SMOKE");

  /** 0xFFFF00. */
  public static final Color YELLOW = new Color(1.0f, 1.0f, 0.0f, "YELLOW");

  /** 0x9ACD32. */
  public static final Color YELLOW_GREEN =
      new Color(0.6039216f, 0.8039216f, 0.19607843f, "YELLOW_GREEN");
}
