// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Signal.hpp"

#include <algorithm>
#include <cmath>
#include <span>
#include <vector>

namespace wpi::filterdesigner {

double Signal::InferSampleRate(std::span<const double> timestamps) {
  if (timestamps.size() < 2) {
    return 0.0;
  }
  std::vector<double> diffs;
  diffs.reserve(timestamps.size() - 1);
  for (size_t i = 1; i < timestamps.size(); ++i) {
    diffs.push_back(timestamps[i] - timestamps[i - 1]);
  }
  auto mid = diffs.begin() + diffs.size() / 2;
  std::nth_element(diffs.begin(), mid, diffs.end());
  double period = *mid;
  return period > 0.0 ? 1.0 / period : 0.0;
}

bool Signal::IsUniform(std::span<const double> timestamps, double tolerance) {
  double rate = InferSampleRate(timestamps);
  if (rate == 0.0) {
    return false;
  }
  double period = 1.0 / rate;
  for (size_t i = 1; i < timestamps.size(); ++i) {
    if (std::abs((timestamps[i] - timestamps[i - 1]) - period) > tolerance) {
      return false;
    }
  }
  return true;
}

}  // namespace wpi::filterdesigner
