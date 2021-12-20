// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import edu.wpi.first.util.CircularBuffer;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * A class that implements a moving-window median filter. Useful for reducing measurement noise,
 * especially with processes that generate occasional, extreme outliers (such as values from vision
 * processing, LIDAR, or ultrasonic sensors).
 */
public class MedianFilter implements Sendable {
  private final CircularBuffer m_values;
  private final List<Double> m_sortedValues;

  public final int m_size;

  private double m_output;

  /**
   * Creates a new MedianFilter.
   *
   * @param size The number of samples in the moving window.
   */
  public MedianFilter(int size) {
    // Circular buffer of values currently in the window, ordered by time
    m_values = new CircularBuffer(size);
    // List of values currently in the window, ordered by value
    m_sortedValues = new ArrayList<>(size);
    // Size of rolling window
    m_size = size;
  }

  /**
   * Calculates the moving-window median for the next value of the input stream.
   *
   * @param next The next input value.
   * @return The median of the moving window, updated to include the next value.
   */
  public double calculate(double next) {
    // Find insertion point for next value
    int index = Collections.binarySearch(m_sortedValues, next);

    // Deal with binarySearch behavior for element not found
    if (index < 0) {
      index = Math.abs(index + 1);
    }

    // Place value at proper insertion point
    m_sortedValues.add(index, next);

    int curSize = m_sortedValues.size();

    // If buffer is at max size, pop element off of end of circular buffer
    // and remove from ordered list
    if (curSize > m_size) {
      m_sortedValues.remove(m_values.removeLast());
      --curSize;
    }

    // Add next value to circular buffer
    m_values.addFirst(next);

    if (curSize % 2 != 0) {
      // If size is odd, return middle element of sorted list
      m_output = m_sortedValues.get(curSize / 2);
    } else {
      // If size is even, return average of middle elements
      m_output = (m_sortedValues.get(curSize / 2 - 1) + m_sortedValues.get(curSize / 2)) / 2.0;
    }

    return m_output;
  }

  /** Resets the filter, clearing the window of all elements. */
  public void reset() {
    m_sortedValues.clear();
    m_values.clear();
  }

  /**
   * Gets a copy of the filter's internal data buffer.
   *
   * @return A copy of the internal data buffer.
   */
  public double[] getData() {
    return m_values.getData();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("MedianFilter");
    builder.addDoubleProperty("size", () -> m_size, null);
    builder.addDoubleArrayProperty("inputs", m_values::getData, null);
    builder.addDoubleProperty("output", () -> m_output, null);
  }
}
