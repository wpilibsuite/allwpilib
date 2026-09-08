// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * The "Filter" wire type: a designed biquad cascade plus the sample rate it
 * was designed at, so a sink cannot disagree with the producing stage about
 * fs. Sources own; sinks borrow by pointer, as with Signal.
 */
struct DesignedFilter {
  Sections sections;
  double sampleRate = 0.0;
};

}  // namespace wpi::filterdesigner
