// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * The "Filter" wire type. A designed biquad cascade plus the sample rate it
 * was designed at — bundling sampleRate with the sections means downstream
 * sinks (Bode plot, code-gen, future filter-merge nodes) don't need a
 * parallel fs field and can't disagree with the producing stage.
 *
 * Sources own; sinks borrow by pointer. Same lifetime contract as Signal.
 */
struct DesignedFilter {
  Sections sections;
  double sampleRate = 0.0;
};

}  // namespace wpi::filterdesigner
