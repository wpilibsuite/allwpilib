// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/TrackWidthAnalysis.h"

#include <cmath>

double sysid::CalculateTrackWidth(double l, double r, units::radian_t accum) {
  // The below comes from solving ω = (vr − vl) / 2r for 2r.
  return (std::abs(r) + std::abs(l)) / std::abs(accum.value());
}
