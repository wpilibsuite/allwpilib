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
class MedianFilter {
 public:
  explicit MedianFilter(size_t size);

  double Calculate(double next);

 private:
  wpi::circular_buffer<double> m_valueBuffer{0};
  std::vector<double> m_orderedValues;
  size_t m_size;
};
}  // namespace frc
