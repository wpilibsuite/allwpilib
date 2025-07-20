// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AddressableLEDJNI;
import edu.wpi.first.hal.HAL;

/**
 * A class for driving addressable LEDs, such as WS2812B, WS2815, and NeoPixels.
 *
 * <p>Some LEDs use a different color order than the default GRB. The color order is configurable
 * using {@link #setColorOrder(ColorOrder)}.
 *
 * <p>Up to 1024 LEDs may be controlled in total across all AddressableLED instances. A single
 * global buffer is used for all instances. The start position used for LED data for the output is
 * set via SetStart() and the length of the strip is set via SetLength(). Both of these default to
 * zero, so multiple instances will access the same pixel data unless SetStart() is called to adjust
 * the starting point.
 */
public class AddressableLED implements AutoCloseable {
  /** Order that color data is sent over the wire. */
  public enum ColorOrder {
    /** RGB order. */
    kRGB(AddressableLEDJNI.COLOR_ORDER_RGB),
    /** RBG order. */
    kRBG(AddressableLEDJNI.COLOR_ORDER_RBG),
    /** BGR order. */
    kBGR(AddressableLEDJNI.COLOR_ORDER_BGR),
    /** BRG order. */
    kBRG(AddressableLEDJNI.COLOR_ORDER_BRG),
    /** GBR order. */
    kGBR(AddressableLEDJNI.COLOR_ORDER_GBR),
    /** GRB order. This is the default order. */
    kGRB(AddressableLEDJNI.COLOR_ORDER_GRB);

    /** The native value for this ColorOrder. */
    public final int value;

    ColorOrder(int value) {
      this.value = value;
    }

    /**
     * Gets a color order from an int value.
     *
     * @param value int value
     * @return color order
     */
    public ColorOrder fromValue(int value) {
      return switch (value) {
        case AddressableLEDJNI.COLOR_ORDER_RBG -> kRBG;
        case AddressableLEDJNI.COLOR_ORDER_BGR -> kBGR;
        case AddressableLEDJNI.COLOR_ORDER_BRG -> kBRG;
        case AddressableLEDJNI.COLOR_ORDER_GRB -> kGRB;
        case AddressableLEDJNI.COLOR_ORDER_GBR -> kGBR;
        case AddressableLEDJNI.COLOR_ORDER_RGB -> kRGB;
        default -> kGRB;
      };
    }
  }

  private final int m_channel;
  private final int m_handle;
  private int m_start;
  private int m_length;
  private ColorOrder m_colorOrder = ColorOrder.kGRB;

  /**
   * Constructs a new driver for a specific channel.
   *
   * @param channel the output channel to use
   */
  public AddressableLED(int channel) {
    m_channel = channel;
    m_handle = AddressableLEDJNI.initialize(channel);
    HAL.reportUsage("IO", channel, "AddressableLED");
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      AddressableLEDJNI.free(m_handle);
    }
  }

  /**
   * Gets the output channel.
   *
   * @return the output channel
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Sets the color order for this AddressableLED. The default order is GRB.
   *
   * <p>This will take effect on the next call to {@link #setData(AddressableLEDBuffer)}.
   *
   * @param order the color order
   */
  public void setColorOrder(ColorOrder order) {
    m_colorOrder = order;
  }

  /**
   * Sets the display start of the LED strip in the global buffer.
   *
   * @param start the strip start, in LEDs
   */
  public void setStart(int start) {
    m_start = start;
    AddressableLEDJNI.setStart(m_handle, start);
  }

  /**
   * Gets the display start of the LED strip in the global buffer.
   *
   * @return the strip start, in LEDs
   */
  public int getStart() {
    return m_start;
  }

  /**
   * Sets the length of the LED strip.
   *
   * @param length the strip length, in LEDs
   */
  public void setLength(int length) {
    m_length = length;
    AddressableLEDJNI.setLength(m_handle, length);
  }

  /**
   * Sets the LED output data.
   *
   * <p>This will write to the global buffer starting at the location set by setStart() and up to
   * the length set by setLength().
   *
   * @param buffer the buffer to write
   */
  public void setData(AddressableLEDBuffer buffer) {
    AddressableLEDJNI.setData(
        m_start,
        m_colorOrder.value,
        buffer.m_buffer,
        0,
        3 * Math.min(m_length, buffer.getLength()));
  }

  /**
   * Sets the LED output data at an arbitrary location in the global buffer.
   *
   * @param start the start location, in LEDs
   * @param colorOrder the color order
   * @param buffer the buffer to write
   */
  public static void setGlobalData(int start, ColorOrder colorOrder, AddressableLEDBuffer buffer) {
    AddressableLEDJNI.setData(start, colorOrder.value, buffer.m_buffer);
  }
}
