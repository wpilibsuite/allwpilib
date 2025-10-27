// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <vector>

#include <wpi/Algorithm.h>
#include <wpi/circular_buffer.h>

namespace frc {
/**
 * A class that implements a moving-window median filter.  Useful for reducing
 * measurement noise, especially with processes that generate occasional,
 * extreme outliers (such as values from vision processing, LIDAR, or ultrasonic
 * sensors).
 */
template <class T>
class MedianFilter {
 public:
  /**
   * Creates a new MedianFilter.
   *
   * @param size The number of samples in the moving window.
   */
  constexpr explicit MedianFilter(size_t size)
      : m_valueBuffer(size), m_size{size} {}

  /**
   * Calculates the moving-window median for the next value of the input stream.
   *
   * @param next The next input value.
   * @return The median of the moving window, updated to include the next value.
   */
  constexpr T Calculate(T next) {
    // Insert next value at proper point in sorted array
    wpi::insert_sorted(m_orderedValues, next);

    size_t curSize = m_orderedValues.size();

    // If buffer is at max size, pop element off of end of circular buffer
    // and remove from ordered list
    if (curSize > m_size) {
      m_orderedValues.erase(std::find(m_orderedValues.begin(),
                                      m_orderedValues.end(),
                                      m_valueBuffer.pop_back()));
      --curSize;
    }

    // Add next value to circular buffer
    m_valueBuffer.push_front(next);

    if (curSize % 2 != 0) {
      // If size is odd, return middle element of sorted list
      return m_orderedValues[curSize / 2];
    } else {
      // If size is even, return average of middle elements
      return (m_orderedValues[curSize / 2 - 1] + m_orderedValues[curSize / 2]) /
             2.0;
    }
  }

  /**
   * Returns the last value calculated by the MedianFilter.
   *
   * @return The last value.
   */
  constexpr T LastValue() const { return m_valueBuffer.front(); }

  /**
   * Resets the filter, clearing the window of all elements.
   */
  constexpr void Reset() {
    m_orderedValues.clear();
    m_valueBuffer.reset();
  }

 private:
  wpi::circular_buffer<T> m_valueBuffer;
  std::vector<T> m_orderedValues;
  size_t m_size;
};
}  // namespace frc
