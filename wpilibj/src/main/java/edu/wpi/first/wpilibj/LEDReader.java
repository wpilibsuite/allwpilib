// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.ErrorMessages;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.function.IntUnaryOperator;

/** Generic interface for reading data from an LED buffer. */
public interface LEDReader {
  /**
   * Gets the length of the buffer.
   *
   * @return the buffer length
   */
  int getLength();

  /**
   * Gets the most recently written color for a particular LED in the buffer.
   *
   * @param index the index of the LED
   * @return the LED color
   * @throws IndexOutOfBoundsException if the index is negative or greater than {@link #getLength()}
   */
  default Color getLED(int index) {
    return new Color(getRed(index) / 255.0, getGreen(index) / 255.0, getBlue(index) / 255.0);
  }

  /**
   * Gets the most recently written color for a particular LED in the buffer.
   *
   * @param index the index of the LED
   * @return the LED color
   * @throws IndexOutOfBoundsException if the index is negative or greater than {@link #getLength()}
   */
  default Color8Bit getLED8Bit(int index) {
    return new Color8Bit(getRed(index), getGreen(index), getBlue(index));
  }

  /**
   * Gets the red channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the red channel, from [0, 255]
   */
  int getRed(int index);

  /**
   * Gets the green channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the green channel, from [0, 255]
   */
  int getGreen(int index);

  /**
   * Gets the blue channel of the color at the specified index.
   *
   * @param index the index of the LED to read
   * @return the value of the blue channel, from [0, 255]
   */
  int getBlue(int index);

  /**
   * A functional interface that allows for iteration over an LED buffer without manually writing an
   * indexed for-loop.
   */
  @FunctionalInterface
  interface IndexedColorIterator {
    /**
     * Accepts an index of an LED in the buffer and the red, green, and blue components of the
     * currently stored color for that LED.
     *
     * @param index the index of the LED in the buffer that the red, green, and blue channels
     *     corresponds to
     * @param r the value of the red channel of the color currently in the buffer at index {@code i}
     * @param g the value of the green channel of the color currently in the buffer at index {@code
     *     i}
     * @param b the value of the blue channel of the color currently in the buffer at index {@code
     *     i}
     */
    void accept(int index, int r, int g, int b);
  }

  /**
   * Iterates over the LEDs in the buffer, starting from index 0. The iterator function is passed
   * the current index of iteration, along with the values for the red, green, and blue components
   * of the color written to the LED at that index.
   *
   * @param iterator the iterator function to call for each LED in the buffer.
   */
  default void forEach(IndexedColorIterator iterator) {
    int bufLen = getLength();
    for (int i = 0; i < bufLen; i++) {
      iterator.accept(i, getRed(i), getGreen(i), getBlue(i));
    }
  }

  /**
   * An LED reader implementation that operates on remapped indices. Remapping can be done using
   * arbitrary mapping functions; a static factory function is also provided for the common use case
   * of offset readers for things like scrolling animations.
   */
  class RemappedReader implements LEDReader {
    private final LEDReader m_reader;
    private final IntUnaryOperator m_mapping;

    /**
     * Creates a new remapping reader for a backing reader and an arbitrary index remapping
     * function.
     *
     * @param reader the backing reader to use
     * @param mapping the mapping function to use
     */
    public RemappedReader(LEDReader reader, IntUnaryOperator mapping) {
      m_reader = ErrorMessages.requireNonNullParam(reader, "reader", "RemappedReader");
      m_mapping = ErrorMessages.requireNonNullParam(mapping, "mapping", "RemappedReader");
    }

    /**
     * Creates a new offset LED reader based on an existing reader and integer offset. The offset
     * reader will be circular and wrap around.
     *
     * @param reader the backing reader to use
     * @param offset the offset to use when reading data
     * @return the offset reader
     */
    public static RemappedReader offset(LEDReader reader, int offset) {
      return new RemappedReader(
          reader, original -> Math.floorMod(original + offset, reader.getLength()));
    }

    @Override
    public int getLength() {
      return m_reader.getLength();
    }

    @Override
    public int getRed(int index) {
      return m_reader.getRed(remapIndex(index));
    }

    @Override
    public int getGreen(int index) {
      return m_reader.getGreen(remapIndex(index));
    }

    @Override
    public int getBlue(int index) {
      return m_reader.getBlue(remapIndex(index));
    }

    /**
     * Remaps an input index to the corresponding offset index.
     *
     * @param index the input index to remap
     * @return the remapped index
     */
    public int remapIndex(int index) {
      return m_mapping.applyAsInt(index);
    }
  }
}
