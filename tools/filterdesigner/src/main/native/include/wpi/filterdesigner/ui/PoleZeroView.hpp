// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * ImGui panel that draws the z-plane pole-zero map of the current biquad
 * cascade. Poles are rendered with crosses, zeros with circles, and the unit
 * circle is overlaid as a reference for stability (poles inside = stable).
 */
class PoleZeroView {
 public:
  /** @param sections Current cascade (nullopt or empty clears the plot). */
  void Display(const std::optional<Sections>& sections);
};

}  // namespace wpi::filterdesigner
