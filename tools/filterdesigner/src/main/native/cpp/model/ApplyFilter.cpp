// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/ApplyFilter.hpp"

#include <vector>

#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::filterdesigner {

std::vector<double> ApplyFilter(std::span<const double> samples,
                                const Sections& sections) {
  if (sections.empty()) {
    return {samples.begin(), samples.end()};
  }
  wpi::math::BiquadFilter filter(sections);
  std::vector<double> out;
  out.reserve(samples.size());
  for (double x : samples) {
    out.push_back(filter.Calculate(x));
  }
  return out;
}

}  // namespace wpi::filterdesigner
