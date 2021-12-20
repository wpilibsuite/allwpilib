// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <vector>

#include <wpi/Algorithm.h>
#include <wpi/circular_buffer.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableHelper.h>
#include <wpi/span.h>

namespace frc {
/**
 * A class that implements a moving-window median filter.  Useful for reducing
 * measurement noise, especially with processes that generate occasional,
 * extreme outliers (such as values from vision processing, LIDAR, or ultrasonic
 * sensors).
 */
template <class T>
class MedianFilter : public wpi::Sendable,
                     public wpi::SendableHelper<MedianFilter<T>> {
 public:
  /**
   * Creates a new MedianFilter.
   *
   * @param size The number of samples in the moving window.
   */
  explicit MedianFilter(size_t size) : m_values(size), m_size{size} {}

  /**
   * Calculates the moving-window median for the next value of the input stream.
   *
   * @param next The next input value.
   * @return The median of the moving window, updated to include the next value.
   */
  T Calculate(T next) {
    // Insert next value at proper point in sorted array
    wpi::insert_sorted(m_sortedValues, next);

    size_t curSize = m_sortedValues.size();

    // If buffer is at max size, pop element off of end of circular buffer
    // and remove from ordered list
    if (curSize > m_size) {
      m_sortedValues.erase(std::find(
          m_sortedValues.begin(), m_sortedValues.end(), m_values.pop_back()));
      --curSize;
    }

    // Add next value to circular buffer
    m_values.push_front(next);

    if (curSize % 2 != 0) {
      // If size is odd, return middle element of sorted list
      m_output = m_sortedValues[curSize / 2];
    } else {
      // If size is even, return average of middle elements
      m_output =
          (m_sortedValues[curSize / 2 - 1] + m_sortedValues[curSize / 2]) / 2.0;
    }

    return m_output;
  }

  /**
   * Resets the filter, clearing the window of all elements.
   */
  void Reset() {
    m_sortedValues.clear();
    m_values.reset();
  }

  /**
   * Gets a copy of the filter's internal data buffer.
   *
   * @return A copy of the internal data buffer.
   */
  std::vector<T> GetData() { return m_values; }

  void InitSendable(wpi::SendableBuilder& builder) override {
    builder.SetSmartDashboardType("MedianFilter");
    builder.AddDoubleProperty(
        "size", [&] { return static_cast<double>(m_size); }, nullptr);
    builder.AddSmallDoubleArrayProperty(
        "inputs",
        [&](wpi::SmallVectorImpl<double>& values) -> wpi::span<double> {
          values.assign(m_values.GetData().begin(), m_values.GetData().end());
          return values;
        },
        nullptr);
    builder.AddDoubleProperty(
        "output", [&] { return static_cast<double>(m_output); }, nullptr);
  }

 private:
  wpi::circular_buffer<T> m_values;
  std::vector<T> m_sortedValues;
  size_t m_size;
  T m_output;
};
}  // namespace frc
