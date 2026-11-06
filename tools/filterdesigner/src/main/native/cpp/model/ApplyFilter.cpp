// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/ApplyFilter.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "wpi/math/filter/BiquadFilter.hpp"

namespace wpi::filterdesigner {

std::vector<double> ApplyFilter(std::span<const double> samples,
                                const Sections& sections, FilterStart start) {
  if (sections.empty()) {
    return {samples.begin(), samples.end()};
  }
  wpi::math::BiquadFilter filter(sections);
  if (start == FilterStart::SteadyState) {
    // The first sample the cascade will actually see. Not samples.front():
    // a non-finite sample never reaches Calculate, so seeding from one would
    // describe a state the filter never occupies.
    const auto seed = std::ranges::find_if(
        samples, [](double x) { return std::isfinite(x); });
    if (seed != samples.end()) {
      filter.Reset(*seed);
      // Reset(value) divides by each section's 1 + a1 + a2, so a cascade with
      // a pole at z = 1 leaves no usable steady state to start from. Falling
      // back to zero state only costs a startup transient, while keeping such
      // a seed would smear a NaN over every remaining sample.
      if (!std::isfinite(filter.LastValue())) {
        filter.Reset();
      }
    }
  }
  std::vector<double> out;
  out.reserve(samples.size());
  for (double x : samples) {
    // A biquad is recursive, so one non-finite sample from the source would
    // not just spoil its own output: it would sit in the section state and
    // turn every later sample non-finite too, blanking the rest of a log from
    // one bad point. Report the gap and step over it instead, leaving the
    // state on the last real sample so the filter resumes when the signal
    // does.
    if (!std::isfinite(x)) {
      out.push_back(std::numeric_limits<double>::quiet_NaN());
      continue;
    }
    out.push_back(filter.Calculate(x));
  }
  return out;
}

}  // namespace wpi::filterdesigner
