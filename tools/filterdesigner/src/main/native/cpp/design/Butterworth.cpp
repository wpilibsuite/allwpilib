// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Zpk.hpp"
#include "wpi/filterdesigner/design/FilterDesign.hpp"

namespace wpi::filterdesigner {

std::optional<Sections> DesignButterworth(FilterKind kind, int order, double fs,
                                          double f1, double f2) {
  if (order < 1 || fs <= 0.0) {
    return std::nullopt;
  }
  const double nyquist = 0.5 * fs;
  if (f1 <= 0.0 || f1 >= nyquist) {
    return std::nullopt;
  }
  if ((kind == FilterKind::BandPass || kind == FilterKind::BandStop) &&
      (f2 <= f1 || f2 >= nyquist)) {
    return std::nullopt;
  }
  return detail::DesignFromAnalogLp(detail::ButterworthPrototype(order), kind,
                                    fs, f1, f2);
}

}  // namespace wpi::filterdesigner
