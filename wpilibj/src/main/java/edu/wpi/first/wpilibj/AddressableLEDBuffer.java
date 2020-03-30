/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

/**
 * Buffer storage for Addressable LEDs.
 */
public class AddressableLEDBuffer {
  byte[] m_buffer;

  /**
   * Constructs a new LED buffer with the specified length.
   *
   * @param length The length of the buffer in pixels
   */
  public AddressableLEDBuffer(int length) {
    m_buffer = new byte[length * 4];
  }

  /**
   * Sets a specific led in the buffer.
   *
   * @param index the index to write
   * @param r     the r value [0-255]
   * @param g     the g value [0-255]
   * @param b     the b value [0-255]
   */
  @SuppressWarnings("ParameterName")
  public void setRGB(int index, int r, int g, int b) {
    m_buffer[index * 4] = (byte) b;
    m_buffer[(index * 4) + 1] = (byte) g;
    m_buffer[(index * 4) + 2] = (byte) r;
    m_buffer[(index * 4) + 3] = 0;
  }

  /**
   * Sets a specific led in the buffer.
   *
   * @param index the index to write
   * @param h     the h value [0-180]
   * @param s     the s value [0-255]
   * @param v     the v value [0-255]
   */
  @SuppressWarnings("ParameterName")
  public void setHSV(final int index, final int h, final int s, final int v) {
    if (s == 0) {
      setRGB(index, v, v, v);
      return;
    }

    final int region = h / 30;
    final int remainder = (h - (region * 30)) * 6;

    final int p = (v * (255 - s)) >> 8;
    final int q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    final int t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
      case 0:
        setRGB(index, v, t, p);
        break;
      case 1:
        setRGB(index, q, v, p);
        break;
      case 2:
        setRGB(index, p, v, t);
        break;
      case 3:
        setRGB(index, p, q, v);
        break;
      case 4:
        setRGB(index, t, p, v);
        break;
      default:
        setRGB(index, v, p, q);
        break;
    }
  }

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index The index to write
   * @param color The color of the LED
   */
  public void setLED(int index, Color color) {
    setRGB(index,
        (int) (color.red * 255),
        (int) (color.green * 255),
        (int) (color.blue * 255));
  }

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index The index to write
   * @param color The color of the LED
   */
  public void setLED(int index, Color8Bit color) {
    setRGB(index, color.red, color.green, color.blue);
  }

  /**
   * Gets the buffer length.
   *
   * @return the buffer length
   */
  public int getLength() {
    return m_buffer.length / 4;
  }

  /**
   * Gets the color at the specified index.
   *
   * @param index the index to get
   * @return the LED color at the specified index
   */
  public Color8Bit getLED8Bit(int index) {
    return new Color8Bit(m_buffer[index * 4 + 2] & 0xFF, m_buffer[index * 4 + 1] & 0xFF,
                         m_buffer[index * 4] & 0xFF);
  }

  /**
   * Gets the color at the specified index.
   *
   * @param index the index to get
   * @return the LED color at the specified index
   */
  public Color getLED(int index) {
    return new Color((m_buffer[index * 4 + 2] & 0xFF) / 255.0,
                     (m_buffer[index * 4 + 1] & 0xFF) / 255.0,
                     (m_buffer[index * 4] & 0xFF) / 255.0);
  }

}
