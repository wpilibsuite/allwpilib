// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

// import edu.wpi.first.wpilibj.util.Color;
// import edu.wpi.first.wpilibj.util.Color8Bit;

/** Buffer storage for Addressable LEDs. */
public class AddressableLEDBuffer implements LEDReader, LEDWriter {
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
   * @param r the r value [0-255]
   * @param g the g value [0-255]
   * @param b the b value [0-255]
   */
  @Override
  public void setRGB(int index, int r, int g, int b) {
    m_buffer[index * 4] = (byte) b;
    m_buffer[(index * 4) + 1] = (byte) g;
    m_buffer[(index * 4) + 2] = (byte) r;
    m_buffer[(index * 4) + 3] = 0;
  }

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index The index to write
   * @param color The color of the LED
   */
  @Override
  public void setLED(int index, Color color) {
    setRGB(index, (int) (color.red * 255), (int) (color.green * 255), (int) (color.blue * 255));
  }

  /**
   * Sets a specific LED in the buffer.
   *
   * @param index The index to write
   * @param color The color of the LED
   */
  @Override
  public void setLED(int index, Color8Bit color) {
    setRGB(index, color.red, color.green, color.blue);
  }

  /**
   * Gets the buffer length.
   *
   * @return the buffer length
   */
  @Override
  public int getLength() {
    return m_buffer.length / 4;
  }

  /**
   * Gets the color at the specified index.
   *
   * @param index the index to get
   * @return the LED color at the specified index
   */
  @Override
  public Color8Bit getLED8Bit(int index) {
    return new Color8Bit(getRed(index), getGreen(index), getBlue(index));
  }

  /**
   * Gets the color at the specified index.
   *
   * @param index the index to get
   * @return the LED color at the specified index
   */
  @Override
  public Color getLED(int index) {
    return new Color(getRed(index) / 255.0, getGreen(index) / 255.0, getBlue(index) / 255.0);
  }

  /**
   * Gets the red channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the red channel, from [0, 255]
   */
  @Override
  public int getRed(int index) {
    return m_buffer[index * 4 + 2] & 0xFF;
  }

  /**
   * Gets the green channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the green channel, from [0, 255]
   */
  @Override
  public int getGreen(int index) {
    return m_buffer[index * 4 + 1] & 0xFF;
  }

  /**
   * Gets the blue channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the blue channel, from [0, 255]
   */
  @Override
  public int getBlue(int index) {
    return m_buffer[index * 4] & 0xFF;
  }

  /**
   * Creates a view of a subsection of this data buffer, starting from (and including) {@code
   * startingIndex} and ending on (and including) {@code endingIndex}. Views cannot be written
   * directly to an {@link AddressableLED}, but are useful tools for logically separating different
   * sections of an LED strip for independent control.
   *
   * @param startingIndex the first index in this buffer that the view should encompass (inclusive)
   * @param endingIndex the last index in this buffer that the view should encompass (inclusive)
   * @return the view object
   */
  public AddressableLEDBufferView createView(int startingIndex, int endingIndex) {
    return new AddressableLEDBufferView(this, startingIndex, endingIndex);
  }
}
