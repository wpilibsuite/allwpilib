// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AddressableLEDJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PWMJNI;

/**
 * A class for driving addressable LEDs, such as WS2812B, WS2815, and NeoPixels.
 *
 * <p>By default, the timing supports WS2812B and WS2815 LEDs, but is configurable using {@link
 * #setBitTiming(int, int, int, int)}
 *
 * <p>Some LEDs use a different color order than the default GRB. The color order is configurable
 * using {@link #setColorOrder(ColorOrder)}.
 *
 * <p>Only 1 LED driver is currently supported by the roboRIO. However, multiple LED strips can be
 * connected in series and controlled from the single driver.
 */
public class AddressableLED implements AutoCloseable {
  /** Order that color data is sent over the wire. */
  public enum ColorOrder {
    /** RGB order. */
    RGB(AddressableLEDJNI.COLOR_ORDER_RGB),
    /** RBG order. */
    RBG(AddressableLEDJNI.COLOR_ORDER_RBG),
    /** BGR order. */
    BGR(AddressableLEDJNI.COLOR_ORDER_BGR),
    /** BRG order. */
    BRG(AddressableLEDJNI.COLOR_ORDER_BRG),
    /** GBR order. */
    GBR(AddressableLEDJNI.COLOR_ORDER_GBR),
    /** GRB order. This is the default order. */
    GRB(AddressableLEDJNI.COLOR_ORDER_GRB);

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
        case AddressableLEDJNI.COLOR_ORDER_RBG -> RBG;
        case AddressableLEDJNI.COLOR_ORDER_BGR -> BGR;
        case AddressableLEDJNI.COLOR_ORDER_BRG -> BRG;
        case AddressableLEDJNI.COLOR_ORDER_GRB -> GRB;
        case AddressableLEDJNI.COLOR_ORDER_GBR -> GBR;
        case AddressableLEDJNI.COLOR_ORDER_RGB -> RGB;
        default -> GRB;
      };
    }
  }

  private final int m_pwmHandle;
  private final int m_handle;

  /**
   * Constructs a new driver for a specific port.
   *
   * @param port the output port to use (Must be a PWM header, not on MXP)
   */
  public AddressableLED(int port) {
    m_pwmHandle = PWMJNI.initializePWMPort(port);
    m_handle = AddressableLEDJNI.initialize(m_pwmHandle);
    HAL.reportUsage("IO", port, "AddressableLED");
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      AddressableLEDJNI.free(m_handle);
    }
    if (m_pwmHandle != 0) {
      PWMJNI.freePWMPort(m_pwmHandle);
    }
  }

  /**
   * Sets the color order for this AddressableLED. The default order is GRB.
   *
   * <p>This will take effect on the next call to {@link #setData(AddressableLEDBuffer)}.
   *
   * @param order the color order
   */
  public void setColorOrder(ColorOrder order) {
    AddressableLEDJNI.setColorOrder(m_handle, order.value);
  }

  /**
   * Sets the length of the LED strip.
   *
   * <p>Calling this is an expensive call, so it's best to call it once, then just update data.
   *
   * <p>The max length is 5460 LEDs.
   *
   * @param length the strip length
   */
  public void setLength(int length) {
    AddressableLEDJNI.setLength(m_handle, length);
  }

  /**
   * Sets the LED output data.
   *
   * <p>If the output is enabled, this will start writing the next data cycle. It is safe to call,
   * even while output is enabled.
   *
   * @param buffer the buffer to write
   */
  public void setData(AddressableLEDBuffer buffer) {
    AddressableLEDJNI.setData(m_handle, buffer.m_buffer);
  }

  /**
   * Sets the bit timing.
   *
   * <p>By default, the driver is set up to drive WS2812B and WS2815, so nothing needs to be set for
   * those.
   *
   * @param highTime0 high time for 0 bit in nanoseconds (default 400 ns)
   * @param lowTime0 low time for 0 bit in nanoseconds (default 900 ns)
   * @param highTime1 high time for 1 bit in nanoseconds (default 900 ns)
   * @param lowTime1 low time for 1 bit in nanoseconds (default 600 ns)
   */
  public void setBitTiming(int highTime0, int lowTime0, int highTime1, int lowTime1) {
    AddressableLEDJNI.setBitTiming(m_handle, highTime0, lowTime0, highTime1, lowTime1);
  }

  /**
   * Sets the sync time.
   *
   * <p>The sync time is the time to hold output so LEDs enable. Default set for WS2812B and WS2815.
   *
   * @param syncTime the sync time in microseconds (default 280 μs)
   */
  public void setSyncTime(int syncTime) {
    AddressableLEDJNI.setSyncTime(m_handle, syncTime);
  }

  /**
   * Starts the output.
   *
   * <p>The output writes continuously.
   */
  public void start() {
    AddressableLEDJNI.start(m_handle);
  }

  /** Stops the output. */
  public void stop() {
    AddressableLEDJNI.stop(m_handle);
  }
}
