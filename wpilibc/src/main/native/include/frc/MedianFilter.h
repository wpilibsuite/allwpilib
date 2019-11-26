/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <wpi/circular_buffer.h>

namespace frc {
/**
 * A class that implements a moving-window median filter.  Useful for reducing
 * measurement noise, especially with processes that generate occasional,
 * extreme outliers (such as values from vision processing, LIDAR, or ultrasonic
 * sensors).
 */
class MedianFilter {
 public:
  /**
   * Creates a new MedianFilter.
   *
   * @param size The number of samples in the moving window.
   */
  explicit MedianFilter(size_t size);

  /**
   * Calculates the moving-window median for the next value of the input stream.
   *
   * @param next The next input value.
   * @return The median of the moving window, updated to include the next value.
   */
  double Calculate(double next);

  /**
   * Resets the filter, clearing the window of all elements.
   */
  void Reset();

 private:
  wpi::circular_buffer<double> m_valueBuffer;
  std::vector<double> m_orderedValues;
  size_t m_size;
};
}  // namespace frc
