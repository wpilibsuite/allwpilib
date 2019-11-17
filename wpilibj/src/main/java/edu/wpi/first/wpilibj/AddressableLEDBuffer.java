/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

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
   * @param r the r value
   * @param g the g value
   * @param b the b value
   */
  @SuppressWarnings("ParameterName")
  public void setLED(int index, int r, int g, int b) {
    m_buffer[index * 4] = (byte) b;
    m_buffer[(index * 4) + 1] = (byte) g;
    m_buffer[(index * 4) + 2] = (byte) r;
    m_buffer[(index * 4) + 3] = 0;
  }
}
