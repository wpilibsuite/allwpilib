// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;

/**
 * A view of another addressable LED buffer. Views CANNOT be written directly to an LED strip; the
 * backing buffer must be written instead. However, views provide an easy way to split a large LED
 * strip into smaller sections (which may be reversed from the orientation of the LED strip as a
 * whole) that can be animated individually without modifying LEDs outside those sections.
 */
public class AddressableLEDBufferView implements LEDReader, LEDWriter {
  private final LEDReader m_backingReader;
  private final LEDWriter m_backingWriter;
  private final int m_startingIndex;
  private final int m_endingIndex;
  private final int m_length;

  /**
   * Creates a new view of a buffer. A view will be reversed if the starting index is after the
   * ending index; writing front-to-back in the view will write in the back-to-front direction on
   * the underlying buffer.
   *
   * @param backingBuffer the backing buffer to view
   * @param startingIndex the index of the LED in the backing buffer that the view should start from
   * @param endingIndex the index of the LED in the backing buffer that the view should end on
   * @param <B> the type of the buffer object to create a view for
   */
  public <B extends LEDReader & LEDWriter> AddressableLEDBufferView(
      B backingBuffer, int startingIndex, int endingIndex) {
    this(
        requireNonNullParam(backingBuffer, "backingBuffer", "AddressableLEDBufferView"),
        backingBuffer,
        startingIndex,
        endingIndex);
  }

  /**
   * Creates a new view of a buffer. A view will be reversed if the starting index is after the
   * ending index; writing front-to-back in the view will write in the back-to-front direction on
   * the underlying buffer.
   *
   * @param backingReader the backing LED data reader
   * @param backingWriter the backing LED data writer
   * @param startingIndex the index of the LED in the backing buffer that the view should start from
   * @param endingIndex the index of the LED in the backing buffer that the view should end on
   */
  public AddressableLEDBufferView(
      LEDReader backingReader, LEDWriter backingWriter, int startingIndex, int endingIndex) {
    requireNonNullParam(backingReader, "backingReader", "AddressableLEDBufferView");
    requireNonNullParam(backingWriter, "backingWriter", "AddressableLEDBufferView");

    if (startingIndex < 0 || startingIndex >= backingReader.getLength()) {
      throw new IndexOutOfBoundsException("Start index out of range: " + startingIndex);
    }
    if (endingIndex < 0 || endingIndex >= backingReader.getLength()) {
      throw new IndexOutOfBoundsException("End index out of range: " + endingIndex);
    }

    m_backingReader = backingReader;
    m_backingWriter = backingWriter;

    m_startingIndex = startingIndex;
    m_endingIndex = endingIndex;
    m_length = Math.abs(endingIndex - startingIndex) + 1;
  }

  /**
   * Creates a view that operates on the same range as this one, but goes in reverse order. This is
   * useful for serpentine runs of LED strips connected front-to-end; simply reverse the view for
   * reversed sections and animations will move in the same physical direction along both strips.
   *
   * @return the reversed view
   */
  public AddressableLEDBufferView reversed() {
    return new AddressableLEDBufferView(this, m_length - 1, 0);
  }

  @Override
  public int getLength() {
    return m_length;
  }

  @Override
  public void setRGB(int index, int r, int g, int b) {
    m_backingWriter.setRGB(nativeIndex(index), r, g, b);
  }

  @Override
  public Color getLED(int index) {
    // override to delegate to the backing buffer to avoid 3x native index lookups & bounds checks
    return m_backingReader.getLED(nativeIndex(index));
  }

  @Override
  public Color8Bit getLED8Bit(int index) {
    // override to delegate to the backing buffer to avoid 3x native index lookups & bounds checks
    return m_backingReader.getLED8Bit(nativeIndex(index));
  }

  @Override
  public int getRed(int index) {
    return m_backingReader.getRed(nativeIndex(index));
  }

  @Override
  public int getGreen(int index) {
    return m_backingReader.getGreen(nativeIndex(index));
  }

  @Override
  public int getBlue(int index) {
    return m_backingReader.getBlue(nativeIndex(index));
  }

  /**
   * Checks if this view is reversed with respect to its backing buffer.
   *
   * @return true if the view is reversed, false otherwise
   */
  public boolean isReversed() {
    return m_endingIndex < m_startingIndex;
  }

  /**
   * Converts a view-local index in the range [start, end] to a global index in the range [0,
   * length].
   *
   * @param viewIndex the view-local index
   * @return the corresponding global index
   * @throws IndexOutOfBoundsException if the view index is not contained within the bounds of this
   *     view
   */
  private int nativeIndex(int viewIndex) {
    if (isReversed()) {
      //  0  1  2  3   4  5  6  7   8  9  10
      //  ↓  ↓  ↓  ↓   ↓  ↓  ↓  ↓   ↓  ↓  ↓
      // [_, _, _, _, (d, c, b, a), _, _, _]
      //               ↑  ↑  ↑  ↑
      //               3  2  1  0
      if (viewIndex < 0 || viewIndex > m_startingIndex) {
        throw new IndexOutOfBoundsException(viewIndex);
      }
      return m_startingIndex - viewIndex;
    } else {
      //  0  1  2  3   4  5  6  7   8  9  10
      //  ↓  ↓  ↓  ↓   ↓  ↓  ↓  ↓   ↓  ↓  ↓
      // [_, _, _, _, (a, b, c, d), _, _, _]
      //               ↑  ↑  ↑  ↑
      //               0  1  2  3
      if (viewIndex < 0 || viewIndex > m_endingIndex) {
        throw new IndexOutOfBoundsException(viewIndex);
      }
      return m_startingIndex + viewIndex;
    }
  }
}
